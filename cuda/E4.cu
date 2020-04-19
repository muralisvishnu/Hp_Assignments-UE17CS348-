#include "utils.h"
#include <thrust/host_vector.h>
#include <thrust/device_vector.h>
#include <device_launch_parameters.h>
#include <device_functions.h>
#include <thrust/sort.h>
__global__ void print_kernel(unsigned int *d_out)
{
  printf("%d ", d_out[threadIdx.x]);
}


__global__ void histo_kernel(unsigned int * d_out, unsigned int* const d_in,unsigned int shift, const unsigned int numElems)
{
  unsigned int mask = 1 << shift;
  int myId = threadIdx.x + blockDim.x * blockIdx.x;
  if (myId >= numElems)  return;
  int bin = (d_in[myId] & mask) >> shift;
  atomicAdd(&d_out[bin], 1);
}

__global__ void sumscan_kernel(unsigned int * d_in, const size_t numBins, const unsigned int numElems)
{
  int myId = threadIdx.x;
  if (myId >= numElems)  return;
  extern __shared__ float sdata[];
  sdata[myId] = d_in[myId];
  __syncthreads();            // make sure entire block is loaded!

  for (int d = 1; d < numBins; d *= 2) {
    if (myId >= d) {
      sdata[myId] += sdata[myId - d];
    }
    __syncthreads();
  }
  if (myId == 0)  d_in[0] = 0;
  else  d_in[myId] = sdata[myId - 1]; //inclusive->exclusive
}

__global__ void makescan_kernel(unsigned int * d_in, unsigned int *d_scan,unsigned int shift, const unsigned int numElems)
{
  unsigned int mask = 1 << shift;
  int myId = threadIdx.x + blockDim.x * blockIdx.x;
  if (myId >= numElems)  return;
  d_scan[myId] = ((d_in[myId] & mask) >> shift) ? 0 : 1;
}

__global__ void move_kernel(unsigned int* const d_inputVals,
  unsigned int* const d_inputPos,
  unsigned int* const d_outputVals,
  unsigned int* const d_outputPos,
  const unsigned int numElems,
  unsigned int* const d_histogram,
  unsigned int* const d_scaned,
  unsigned int shift)
{
  unsigned int mask = 1 << shift;
  int myId = threadIdx.x + blockDim.x * blockIdx.x;
  if (myId >= numElems)  return;
  int des_id = 0;
  if ((d_inputVals[myId] & mask) >> shift) {
    des_id = myId + d_histogram[1] - d_scaned[myId];
  } else {
    des_id = d_scaned[myId];
  }
  d_outputVals[des_id] = d_inputVals[myId];
  d_outputPos[des_id] = d_inputPos[myId];
}

#ifdef USE_THRUST
void your_sort(unsigned int* const d_inputVals,unsigned int* const d_inputPos,unsigned int* const d_outputVals,unsigned int* const d_outputPos,const size_t numElems)
{
  thrust::device_ptr<unsigned int> d_inputVals_p(d_inputVals);
  thrust::device_ptr<unsigned int> d_inputPos_p(d_inputPos);
  thrust::host_vector<unsigned int> h_inputVals_vec(d_inputVals_p,d_inputVals_p + numElems);
  thrust::host_vector<unsigned int> h_inputPos_vec(d_inputPos_p,d_inputPos_p + numElems);
  thrust::sort_by_key(h_inputVals_vec.begin(), h_inputVals_vec.end(), h_inputPos_vec.begin());
  checkCudaErrors(cudaMemcpy(d_outputVals, thrust::raw_pointer_cast(&h_inputVals_vec[0]),numElems * sizeof(unsigned int), cudaMemcpyHostToDevice));
  checkCudaErrors(cudaMemcpy(d_outputPos, thrust::raw_pointer_cast(&h_inputPos_vec[0]),numElems * sizeof(unsigned int), cudaMemcpyHostToDevice));
}
#else
void your_sort(unsigned int* const d_inputVals,
  unsigned int* const d_inputPos,
  unsigned int* const d_outputVals,
  unsigned int* const d_outputPos,
  const size_t numElems)
{
  const int numBits = 1;  //??
  const int numBins = 1 << numBits;
  const int m = 1 << 10;
  int blocks = ceil((float)numElems / m);
  printf("m %d blocks %d\n", m ,blocks);
  unsigned int *d_binHistogram;
  checkCudaErrors(cudaMalloc(&d_binHistogram, sizeof(unsigned int)* numBins));
  thrust::device_vector<unsigned int> d_scan(numElems);
  for (unsigned int i = 0; i < 8 * sizeof(unsigned int); i++) {
    checkCudaErrors(cudaMemset(d_binHistogram, 0, sizeof(unsigned int)* numBins));
    histo_kernel << <blocks, m >> >(d_binHistogram, d_inputVals, i, numElems);
    cudaDeviceSynchronize();
    checkCudaErrors(cudaGetLastError());
    sumscan_kernel << <1, numBins, sizeof(unsigned int)* numBins>> >(d_binHistogram, numBins, numElems);
    cudaDeviceSynchronize();
    checkCudaErrors(cudaGetLastError());
    makescan_kernel << <blocks, m >> >(d_inputVals, thrust::raw_pointer_cast(&d_scan[0]), i, numElems);
    cudaDeviceSynchronize();
    checkCudaErrors(cudaGetLastError());
    thrust::exclusive_scan(d_scan.begin(), d_scan.end(), d_scan.begin());
    cudaDeviceSynchronize();
    checkCudaErrors(cudaGetLastError());
    move_kernel << <blocks, m >> >(d_inputVals, d_inputPos, d_outputVals, d_outputPos,numElems, d_binHistogram, thrust::raw_pointer_cast(&d_scan[0]), i);
    cudaDeviceSynchronize();
    checkCudaErrors(cudaGetLastError());
    checkCudaErrors(cudaMemcpy(d_inputVals, d_outputVals, numElems * sizeof(unsigned int), cudaMemcpyDeviceToDevice));
    checkCudaErrors(cudaMemcpy(d_inputPos, d_outputPos, numElems * sizeof(unsigned int), cudaMemcpyDeviceToDevice));
    cudaDeviceSynchronize();
    checkCudaErrors(cudaGetLastError());
  }
  checkCudaErrors(cudaFree(d_binHistogram));
}
#endif