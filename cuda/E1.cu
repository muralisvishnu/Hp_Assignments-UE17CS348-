#include "reference_calc.cpp"
#include "utils.h"
#include <stdio.h>

__global__
void rgb_grey(const uchar4* const rgbaImage,unsigned char* const greyImage,int nr, int nc)
{
      int indx.x = threadIdx.x;  
      int indx.y = threadIdx.y;
      int bindx.x = blockIdx.x;
      int bindx.y = blockIdx.y;
      
      int bdim.x = blockDim.x;
      int bdim.y = blockDim.y; 
      int gdim.x = gridDim.x;
      int gdim.y = gridDim.y;
      
      int xp = bdim.x * bindx.x + indx.x;
      int yp = bdim.y * bindx.y + indx.y;
          
      int offset =  yp * (bdim.x * gdim.x) + xp;
      
      uchar4 rgb = rgbaImage[offset];
      float chSum = .299f * rgb.x + .587f * rgb.y + .114f * rgb.z;
      greyImage[offset] = chSum; 
    
}

void rgb_grey1(const uchar4 * const h_rgbaImage, uchar4 * const d_rgbaImage,unsigned char* const d_greyImage, size_t nr, size_t nc)
{
  
  const dim3 blockSize(nr/16+1, nc/16+1, 1);  //TODO
  const dim3 gridSize( 16, 16, 1);  //TODO
  rgb_grey<<<gridSize, blockSize>>>(d_rgbaImage, d_greyImage, nr, nc);
  cudaDeviceSynchronize();
  checkCudaErrors(cudaGetLastError());
}