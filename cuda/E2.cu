#include "reference_calc.cpp"
#include "utils.h"

__global__
void gb(const unsigned char* const ic,unsigned char* const oc,int nr, int nc,const float* const filter, const int filterWidth)                                      
{
  const int2 thread_2D_pos = make_int2( blockIdx.x * blockDim.x + threadIdx.x,blockIdx.y * blockDim.y + threadIdx.y);
  const int thread_1D_pos = thread_2D_pos.y * nc + thread_2D_pos.x;
  if (thread_2D_pos.x >= nc || thread_2D_pos.y >= nr)
    return;
  float result = 0.f;
  for (int filter_r = -filterWidth/2; filter_r <= filterWidth/2; ++filter_r) {
    for (int filter_c = -filterWidth/2; filter_c <= filterWidth/2; ++filter_c) {
      int image_r = min(max(thread_2D_pos.y + filter_r, 0), static_cast<int>(nr - 1));
      int image_c = min(max(thread_2D_pos.x + filter_c, 0), static_cast<int>(nc - 1));

      float image_value = static_cast<float>(ic[image_r * nc + image_c]);
      float filter_value = filter[(filter_r + filterWidth/2) * filterWidth + filter_c + filterWidth/2];

      result += image_value * filter_value;
    }
  }
  oc[thread_1D_pos] = result;
}
__global__
void sc(const uchar4* const iprgb,int nr,int nc,unsigned char* const rc,unsigned char* const gc,unsigned char* const bc)
{
  const int2 thread_2D_pos = make_int2( blockIdx.x * blockDim.x + threadIdx.x,blockIdx.y * blockDim.y + threadIdx.y);
  const int thread_1D_pos = thread_2D_pos.y * nc + thread_2D_pos.x;
  if (thread_2D_pos.x >= nc || thread_2D_pos.y >= nr)
    return;rc[thread_1D_pos] = iprgb[thread_1D_pos].x;gc[thread_1D_pos] = iprgb[thread_1D_pos].y;bc[thread_1D_pos] = iprgb[thread_1D_pos].z;
}
__global__
void recombineChannels(const unsigned char* const rc,const unsigned char* const gc,const unsigned char* const bc,uchar4* const oprgb,int nr,int nc)
{
  const int2 thread_2D_pos = make_int2( blockIdx.x * blockDim.x + threadIdx.x,blockIdx.y * blockDim.y + threadIdx.y);
  const int thread_1D_pos = thread_2D_pos.y * nc + thread_2D_pos.x;
  if (thread_2D_pos.x >= nc || thread_2D_pos.y >= nr)
    return;
  unsigned char red   = rc[thread_1D_pos];
  unsigned char green = gc[thread_1D_pos];
  unsigned char blue  = bc[thread_1D_pos];
  uchar4 outputPixel = make_uchar4(red, green, blue, 255);
  oprgb[thread_1D_pos] = outputPixel;
}
unsigned char *d_red, *d_green, *d_blue;
float *d_filter;
void allocateMemoryAndCopyToGPU(const size_t nrImage, const size_t ncImage,const float* const h_filter, const size_t filterWidth)
{
  checkCudaErrors(cudaMalloc(&d_red,   sizeof(unsigned char) * nrImage * ncImage));
  checkCudaErrors(cudaMalloc(&d_green, sizeof(unsigned char) * nrImage * ncImage));
  checkCudaErrors(cudaMalloc(&d_blue,  sizeof(unsigned char) * nrImage * ncImage));
  int num_filter_bytes = sizeof(float) * filterWidth * filterWidth;
  checkCudaErrors(cudaMalloc(&d_filter, num_filter_bytes));
  checkCudaErrors(cudaMemcpy(d_filter, h_filter, num_filter_bytes, cudaMemcpyHostToDevice));
}
void your_gb(const uchar4 * const h_iprgb, uchar4 * const d_iprgb,uchar4* const d_oprgb, const size_t nr, const size_t nc,unsigned char *d_redBlurred, unsigned char *d_greenBlurred, unsigned char *d_blueBlurred,const int filterWidth)
{
  const dim3 blockSize(1, 1, 1);
  const dim3 gridSize(nc, nr, 1);
  sc<<<gridSize, blockSize>>>(d_iprgb,nr,nc,d_red,d_green,d_blue);
  cudaDeviceSynchronize(); checkCudaErrors(cudaGetLastError());
  gb<<<gridSize, blockSize>>>(d_red,d_redBlurred,nr,nc,d_filter,filterWidth);
  gb<<<gridSize, blockSize>>>(d_green,d_greenBlurred,nr,nc,d_filter,filterWidth);
  gb<<<gridSize, blockSize>>>(d_blue,d_blueBlurred,nr,nc,d_filter,filterWidth);
  cudaDeviceSynchronize(); checkCudaErrors(cudaGetLastError());
  recombineChannels<<<gridSize, blockSize>>>(d_redBlurred,d_greenBlurred,d_blueBlurred,d_oprgb,nr,nc);
  cudaDeviceSynchronize(); checkCudaErrors(cudaGetLastError());

}
void cleanup() {
  checkCudaErrors(cudaFree(d_red));
  checkCudaErrors(cudaFree(d_green));
  checkCudaErrors(cudaFree(d_blue));
  checkCudaErrors(cudaFree(d_filter));
}