#include <stdio.h>
#include <omp.h>
int main(){
    static long ns = 10000;
    double sum = 0.0;
    double pi=0.0;
    #pragma omp parallel for reduction(+:sum) schedule(runtime)
    for (long c =0; c < ns; c += 1){
            double i = (double)(c)*(1.0/ns);
            sum += 4.0/(1.0+i*i);
    }
    pi=sum*(1.0/ns);
    printf("%lf\n",pi);
}