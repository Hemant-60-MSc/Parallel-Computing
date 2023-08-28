#include "cuda_runtime.h"
#include "device_launch_parameters.h"

#include <stdio.h>
#include <math.h>

float get_val(int i, int j, int n);
void initialize(float* mat, int size);
void print_mat(float* mat, int size);
bool check_diff(float* mat1, float* mat2, float threshold, int size);
void apply_jacobi(float* mat1, int size);


int main(){

    //////////////////////////////////////
    //Initialization
    //////////////////////////////////////
    int n =512 ;
    float mat[n*n];
    initialize(mat, n);


    //////////////////////////////////////
    //Calculation
    //////////////////////////////////////
    
    apply_jacobi(mat,n);

    //////////////////////////////////////
    //Termination
    //////////////////////////////////////
    return 0;
}

float get_val(int i, int j, int n){
    float common_factor = 4*M_PI/(n - 1 );
    return cos(common_factor * i)*cos(common_factor * j);
}

void initialize(float* mat, int size){

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

bool check_diff(float* mat1, float* mat2, float threshold, int size){
    for(int i=1;i<size-1;i++)
        for(int j=1;j<size-1;j++)
            if (fabsf(mat1[i*size + j] - mat2[i*size + j]) > threshold)
                return false;
    
    return true;
}

void apply_jacobi(float* mat, int size){

    float temp[size*size];
    initialize(temp, size);

    while(true){

        for(int i=1; i<size-1;i++){
            for (int j =1; j< size-1;j++){
                float numerator = mat[(i-1)*size + j];
                numerator += mat[(i+1)*size + j];
                numerator += mat[(i)*size + j-1];
                numerator += mat[(i)*size + j+1];
                temp[i*size + j] =  numerator/4;
            }
        }

        if (check_diff(temp, mat, 0.01, size))
            break;

        for(int i=1; i<size-1;i++){
            for (int j =1; j< size-1;j++){
                mat[i*size + j] = temp[i*size + j];
            }
        }

    }
}