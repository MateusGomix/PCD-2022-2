#include <omp.h>
#include <stdio.h>

void main(){
    int t, x;
    t = omp_get_num_threads();

    #pragma omp parallel num_threads(3)
    {
        x = omp_get_num_threads();
    }
    printf("%d %d\n", t, x);
}