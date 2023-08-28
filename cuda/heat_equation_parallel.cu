#include "cuda_runtime.h"
#include "device_launch_parameters.h"

#include <stdio.h>
#include <math.h>

float get_val(int i, int j, int n);
void initialize(float* mat, int size);
void print_mat(float* mat, int size);
// bool check_diff(float* mat1, float* mat2, float threshold, int size);
// void apply_jacobi(float* mat1, int size);

__global__ void cudaInitialize(float* mat, int size){
    int id = blockIdx.x * blockDim.x + threadIdx.x;
    // __shared__ float common_factor;
    // common_factor = (4*M_PI/(size - 1 ));
    // printf("%d\n",id);

    if(id >0 && id<size-1){
        for(int j = 0;j<size;j++){
            if(j ==0 || j==size-1)
                mat[id*size + j] =0.0;
            else
                mat[id*size + j] = cos((4*M_PI/(size - 1 )) * (id))*cos((4*M_PI/(size - 1 )) * j);
        }
    }else{
        for(int j = 0;j<size;j++){
            mat[id*size + j] = 0.0;
        }
    }
    __syncthreads();
    for(int j=0;j<size;j++){
        printf("(%d, %f) ",id, mat[id*size + j]);
    }
}

int main(){

    //////////////////////////////////////
    //Initialization
    //////////////////////////////////////
    int n =4 ;
    float mat[n*n];
    // initialize(mat, n);

    //////////////////////////////////////
    //Initializing matrix
    //////////////////////////////////////
    float* cuda_mat = nullptr;
    cudaMalloc((void**)&cuda_mat, n * sizeof(float));
    cudaMemcpy(cuda_mat, mat, n * sizeof(float), cudaMemcpyHostToDevice);
    cudaInitialize<<<1, n>>>(cuda_mat, n);
    cudaDeviceSynchronize();
    cudaMemcpy(mat, cuda_mat, n * sizeof(int), cudaMemcpyDeviceToHost);
    cudaFree(cuda_mat);

    print_mat(mat, n);

    //////////////////////////////////////
    //Calculation
    //////////////////////////////////////
    
    // apply_jacobi(mat,n);

    // //////////////////////////////////////
    // //Termination
    // //////////////////////////////////////
    // return 0;
}

float get_val(int i, int j, int n){
    float common_factor = 4*M_PI/(n - 1 );
    return cos(common_factor * i)*cos(common_factor * j);
}

void initialize(float* mat, int size){
    // int id = blockIdx.x * blockDim.x + threadIdx.x;
    for(int i=0;i<size;i++){
        for(int j =0; j< size ;j++){
            if(i==0 || i == size-1 || j==0 || j==size-1){
                mat[i*size + j] = 0.0;
            }
            else{
                mat[i*size + j] = get_val(i, j, size);
            }
        }
    }
}


void print_mat(float* mat, int size){
    for(int i=0;i<size;i++){
        for(int j =0; j<size ;j++){
            printf("%f ", mat[i*size + j]);
        }
        printf("\n");
    }
    printf("\n");
}

// bool check_diff(float* mat1, float* mat2, float threshold, int size){
//     for(int i=1;i<size-1;i++)
//         for(int j=1;j<size-1;j++)
//             if (fabsf(mat1[i*size + j] - mat2[i*size + j]) > threshold)
//                 return false;
    
//     return true;
// }

// __global__ void apply_jacobi(float* mat, int size, float threshold){

//     __shared__ float temp[size*size];
//     __shared__ bool flag;
//     int id = blockIdx.x * blockDim.x + threadIdx.x;

//     for(int j=0;j<size;j++){
//         temp[(id+1)*size + j] = 0.00;
//         temp[j] = 0.00;
//         temp[(size-1)*size + j] = 0.00;
//     }

//     __syncthreads();

//     while(true){

//             flag = true;
//             for (int j =1; j< size-1;j++){
//                 float numerator = mat[(i-1)*size + j];
//                 numerator += mat[(id+1)*size + j];
//                 numerator += mat[(id)*size + j-1];
//                 numerator += mat[(id)*size + j+1];
//                 temp[i*size + j] =  numerator/4;
//             }
        
//         __syncthreads();

//         for(int j = 1;j<size-1;j++){
//             if (mat[id][j] > threshold){
//                 flag = false;
//             } 
//         }

//         __syncthreads();

//         if(!flag) break;


//         // for(int i=1; i<size-1;i++){
//             for (int j =1; j< size-1;j++){
//                 mat[id*size + j] = temp[i*size + j];
//             }
//         // }

//     }
// }