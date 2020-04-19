#include <omp.h>
#ifdef APPLE
#include <stdlib.h>
#else
#include <malloc.h>
#endif
#include <stdio.h>
#define N        10000
#define SEED       2531
#define RAND_MULT  1366
#define RAND_ADD   150889
#define RAND_MOD   714025
int randy = SEED;
void fill_rand(int length, double *a)
{
   int i;
   for (i=0;i<length;i++) {
     randy = (RAND_MULT * randy + RAND_ADD) % RAND_MOD;
     *(a+i) = ((double) randy)/((double) RAND_MOD);
   }
}
double Sum_array(int length, double *a)
{
   int i;  double sum = 0.0;
  
   for (i=0;i<length;i++) {
     sum += *(a+i);
   }  
   return sum; 
}
  
int main()
{
  double *A, sum, runtime;
  int flag = 0;
  A = (double *)malloc(N*sizeof(double));
  #pragma omp parallel
  {
    #pragma omp sections
    {
      #pragma omp section
      {
          fill_rand(N, A);
          #pragma omp flush
          #pragma omp atomic write
          flag = 1;
          #pragma omp flush(flag)
      }
      #pragma omp section
      {
        int temp;
        while (1){
          #pragma omp flush(flag)
          #pragma omp atomic read
          temp = flag;
          if (temp == 1) break;
        }
          #pragma omp flush
          sum = Sum_array(N, A);
      }
    }
  }
  printf("%f \n",sum);
}
 
