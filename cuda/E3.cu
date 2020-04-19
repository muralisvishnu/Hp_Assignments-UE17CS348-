#include "utils.h"
#include<device_launch_parameters.h>
#include<device_functions.h>

__global__ void shmem_reduce_kernel(float * d_out, const float * const d_in, bool is_max)
{
  extern __shared__ float sdata[];

  int myId = threadIdx.x + blockDim.x * blockIdx.x;
  int tid = threadIdx.x;
  sdata[tid] = d_in[myId];
  __syncthreads();            // make sure entire block is loaded!
  for (unsigned int s = blockDim.x / 2; s > 0; s >>= 1)
  {
    if (tid < s)
    {
      if (is_max)
        sdata[tid] = max(sdata[tid], sdata[tid + s]);
      else
        sdata[tid] = min(sdata[tid], sdata[tid + s]);
    }
    __syncthreads();        // make sure all adds at one stage are done!
  }
  if (tid == 0)
  {
    d_out[blockIdx.x] = sdata[0];
  }
}

__global__ void histo_kernel(unsigned int * d_out, const float * const d_in,
  const size_t numBins, float logLumRange, float min_logLum)
{
  int myId = threadIdx.x + blockDim.x * blockIdx.x;
  int bin = (d_in[myId] - min_logLum) / logLumRange * numBins;
  if (bin == numBins)  bin--;
  atomicAdd(&d_out[bin], 1);
}

__global__ void scan_kernel(unsigned int * d_out, const float * const d_in,
  const size_t numBins, float logLumRange, float min_logLum)
{
  int myId = threadIdx.x + blockDim.x * blockIdx.x;
  int bin = (d_in[myId] - min_logLum) / logLumRange * numBins;
  if (bin == numBins)  bin--;
  atomicAdd(&d_out[bin], 1);
}
__global__ void cdf_kernel(unsigned int * d_in, const size_t numBins)
{
  int myId = threadIdx.x;
  for (int d = 1; d < numBins; d *= 2) {
    if ((myId + 1) % (d * 2) == 0) {
      d_in[myId] += d_in[myId - d];
    }
    __syncthreads();
  }
  if (myId == numBins - 1) d_in[myId] = 0;
  for (int d = numBins / 2; d >= 1; d /= 2) {
    if ((myId + 1) % (d * 2) == 0) {
      unsigned int tmp = d_in[myId - d];
      d_in[myId - d] = d_in[myId];
      d_in[myId] += tmp;
    }
    __syncthreads();
  }
}
__global__ void cdf_kernel_2(unsigned int * d_in, const size_t numBins)
{ 
  int idx = threadIdx.x;
  extern __shared__ int temp[];
  int pout = 0, pin = 1;

  temp[idx] = (idx > 0) ? d_in[idx - 1] : 0;
  __syncthreads();

  for (int offset = 1; offset < n; offset *= 2) {
    pout = 1 - pout;
    pin = 1 - pout;
    if (idx >= offset) {
      temp[pout*n+idx] = temp[pin*n+idx - offset] + temp[pin*n+idx];  // changed line
    } else {
      temp[pout*n+idx] = temp[pin*n+idx];
    }
    __syncthreads();
  }
  d_in[idx] = temp[pout*n+idx];
}

void your_histogram_and_prefixsum(const float* const d_logLuminance,
  unsigned int* const d_cdf,
  float &min_logLum,
  float &max_logLum,
  const size_t numRows,
  const size_t numCols,
  const size_t numBins)
{
  const int m = 1 << 10;
  int blocks = ceil((float)numCols * numRows / m);

  float *d_intermediate; // should not modify d_in
  checkCudaErrors(cudaMalloc(&d_intermediate, sizeof(float)* blocks)); // store max and min
  float *d_min, *d_max;
  checkCudaErrors(cudaMalloc((void **)&d_min, sizeof(float)));
  checkCudaErrors(cudaMalloc((void **)&d_max, sizeof(float)));

  shmem_reduce_kernel << <blocks, m, m * sizeof(float) >> >(d_intermediate, d_logLuminance, true);
  shmem_reduce_kernel << <1, blocks, blocks * sizeof(float) >> >(d_max, d_intermediate, true);
  shmem_reduce_kernel << <blocks, m, m * sizeof(float) >> >(d_intermediate, d_logLuminance, false);
  shmem_reduce_kernel << <1, blocks, blocks * sizeof(float) >> >(d_min, d_intermediate, false);
  checkCudaErrors(cudaMemcpy(&min_logLum, d_min, sizeof(float), cudaMemcpyDeviceToHost));
  checkCudaErrors(cudaMemcpy(&max_logLum, d_max, sizeof(float), cudaMemcpyDeviceToHost));

  checkCudaErrors(cudaFree(d_intermediate));
  checkCudaErrors(cudaFree(d_min));
  checkCudaErrors(cudaFree(d_max));
  float logLumRange = max_logLum - min_logLum;
  printf("max_logLum: %f  min_logLum: %f  logLumRange: %f\n", max_logLum, min_logLum, logLumRange);
  checkCudaErrors(cudaMemset(d_cdf, 0, sizeof(unsigned int)* numBins));
  histo_kernel << <blocks, m >> >(d_cdf, d_logLuminance, numBins, logLumRange, min_logLum);
  cdf_kernel_2 << <1, numBins, sizeof(unsigned int) * numBins * 2 >> >(d_cdf, numBins);
}