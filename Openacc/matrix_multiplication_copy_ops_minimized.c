#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "openacc.h"

#define N 1000

void print_mat(int** arr, int size){
    for(int i=0;i<N;i++){
        for(int j =0; j<N;j++){
            printf("%d ",arr[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

int main() {

    long arr_sum = 0;
   
    float mat1[N][N], res[N][N];
    #pragma acc enter data create(mat1[0:N][0:N]) //pcreate(mat1[0:N][0:N], mat2[0:N][0:N])
    {   

        #pragma acc parallel loop
        for(int i =0;i<N;i++){
            for(int j =0;j<N;j++){
                double p_val = 12.0 * (double)(i+j)/N; 
                float val=cos(p_val);
                p_val = 24.0 * (double)(i+j)/N;
                val*=p_val;
                mat1[i][j] = val;
            }
        } 
        


        for(int i=0;i<N;i++){
            #pragma acc parallel loop reduction(+:arr_sum) private(arr_sum)
            for(int j=0;j<N;j++){
                arr_sum = 0;
                
                for(int k =0; k< N;k++){
                    arr_sum += mat1[i][k] * mat1[k][j];
                }
                res[i][j] = arr_sum;
            }
        }
    }
    
}

// gcc -fopenacc 1_c.c -o 1_c -lm && time ./1_c