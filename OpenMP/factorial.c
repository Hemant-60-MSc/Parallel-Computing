/*
a) parallel directive parallelize the code portion by spawning threads.
omp for divides the loop iteration between created threads.
omp parallel for is for calling aforementioned together

b) Data dependence when threads address some dependent
memory spaces. Which requires for a specific thread to be
executed after some specific thread(s). Loop dependence refers
to when iterations do not have data dependencies. In our 
case garbage value may be picked from unprocessed
memory location and a garbage value may be produced or in
cases like arraylist, some index may not be yet exisiting.

c) Garbage value is processed and printed out to console.

d) Using for ordered directive before the loop and 
#pragma omp ordered before conditional in the below code.

e) static: 0, 0, 1, 1, 2, 2, 3, 3
dynamic: 0, 3, 1, 2,0, 3, 1, 2
guided: 0, 0, 2, 2, 1, 3, 0, 2
runtime: 2, 0, 1, 3, 2, 0, 1, 3
auto: 0, 0, 1, 1, 2, 2, 3, 3

*/
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

void factorial(int x[], int n);

int main (int argc, const char * argv[])
{
    int arr[8];
    factorial(arr, 8);
    
    return 0;
}

void factorial(int x[], int n)
{
    int i,j,thread_id=42;
    
    #pragma omp parallel for ordered num_threads(4) \
        default(shared) private(i,j) shared (n,x) schedule(auto)
    for(i=0; i<n; i++)
    {
        #pragma omp ordered
        if(i<2)
            x[i] = 1;
        else
         x[i] = x[i-1]*i;
        thread_id=omp_get_thread_num();
        printf("Thread id # %d computed factorial(%d) = %d \n",thread_id, i, x[i]);
    }
    
    for(j=0; j<n; j++)
        printf("%d\t",x[j]);
    printf("\n");
}


