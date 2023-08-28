#include <stdio.h>
#include "openacc.h"

#define N 100
int array[N];
int main() {

    int arr_sum = 0;

    #pragma acc parallel loop
    for(int i=0;i<N;i++){
        array[i] = i;
    }

    #pragma acc parallel loop reduction(+: arr_sum) //copy(array[0:N])
    for(int i = 0; i < N; i++) {
        arr_sum += array[i];
    }
   printf("Sum is %d\n",arr_sum);
}


// gcc -fopenacc 1_a.c -o 1_a && ./1_a