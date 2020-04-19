#include <stdio.h>
#include <omp.h>
int main(){
    static long ns = 10000;
    int talloc = 0;
    double s = 1.0/(double)ns;
    omp_set_num_threads(8);
    double pi = 0.0;
    #pragma omp parallel
    {
        int threadid = omp_get_thread_num();
        int nt = omp_get_num_threads();
        if (threadid ==0) talloc = omp_get_num_threads();
        long c = 0; double i;
        double sum = 0.0;
        for (c = threadid; c < ns; c += nt){
            i = (double)(c)*s;
            sum += (4.0/(1.0+i*i)) ;
        }
        #pragma omp atomic
        pi+=sum*s;
    }
    printf("%lf\n",pi);
}

