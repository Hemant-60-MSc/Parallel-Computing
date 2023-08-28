#include "cuda_runtime.h"
#include "device_launch_parameters.h"

#include <stdio.h>
#include <math.h>


#ifndef __CUDACC__  
    #define __CUDACC__ 
#endif  


__global__ void add(int* arr, int size) {
    int fi = blockIdx.x * blockDim.x + threadIdx.x;
    if (fi< (size/2) ){
        arr[fi] += arr[(size/2) + fi];
    }
    // __sync_threads();
}
void arraySum(int* arr, int size) {
    int* cuda_arr = nullptr;
    cudaMalloc((void**)&cuda_arr, size * sizeof(int));

    cudaMemcpy(cuda_arr, arr, size * sizeof(int), cudaMemcpyHostToDevice);
    int iter_size = size;
    // printf("%d",((int)log2(size)+1));
    for(int i =0;i <= (int)log2(size)+1;i++){
        add<<<2, (iter_size/2 + 1) / 2>>>(cuda_arr, iter_size);
        // cudaDeviceSynchronize();
        // __sync_threads();
        iter_size/=2;
    }

    cudaMemcpy(arr, cuda_arr, size * sizeof(int), cudaMemcpyDeviceToHost);

    cudaFree(cuda_arr);

}

int main(int argc, char** argv) {
    // const int size = 8;
    // int arr[size] = {  1,  2,  3,  4,  5, 6, 7, 8 };

    const int size = 64;
    int arr[size];
    for (int i =0;i< size;i++) arr[i] = 5*i;

    arraySum(arr, size);

    printf("Sum is %d \n", arr[0]);

    cudaDeviceReset();

    return 0;
}