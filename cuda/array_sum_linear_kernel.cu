#include "cuda_runtime.h"
#include "device_launch_parameters.h"

#include <stdio.h>
#include <math.h>

__global__ void add(int* arr, int size) {
    for(int i=0;i<size;i++) arr[size]+=arr[i];
}
void arraySum(int* arr, int size) {
    int* cuda_arr = nullptr;
    size+=1;
    cudaMalloc((void**)&cuda_arr, size * sizeof(int));
    cudaMemcpy(cuda_arr, arr, size * sizeof(int), cudaMemcpyHostToDevice);
    add<<<1,1>>>(cuda_arr, size-1);
    cudaDeviceSynchronize();
    cudaMemcpy(arr, cuda_arr, size * sizeof(int), cudaMemcpyDeviceToHost);
    cudaFree(cuda_arr);
}

int main(int argc, char** argv) {
    const int size = 64;
    int arr[size+1];
    for (int i =0;i< size;i++) arr[i] = 5*i;
    arr[size] = 0;
    // int arr[size+1] = {  1,  2,  3,  4,  5, 6, 7, 8, 0};

    arraySum(arr, size);

    printf("Sum is %d \n", arr[size]);

    cudaDeviceReset();

    return 0;
}