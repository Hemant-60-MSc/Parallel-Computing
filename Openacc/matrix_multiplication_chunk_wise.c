#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "openacc.h"

#define N 100

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

    // int **mat1 = (int **)malloc(N * sizeof(int *));
    // int **mat2 = (int **)malloc(N * sizeof(int *));
    // int **res = (int **)malloc(N * sizeof(int *));
    
    // for (int r = 0; r < N; r++) {
    //     mat1[r] = (int *)malloc(N * sizeof(int));
    //     mat2[r] = (int *)malloc(N * sizeof(int));
    //     res[r] = (int *)malloc(N * sizeof(int));
    // }

    int mat1[N][N], mat2[N][N], res[N][N];

    #pragma acc parallel loop
    for(int i=0;i<N;i++){

        #pragma acc  loop
        for(int j =0; j<N;j++){
            mat1[i][j] = rand()%10;
            mat2[i][j] = rand()%10;
        }
    }

    //////////////////////////
    // Transposing 2nd matrix
    //////////////////////////

    // for(int i= 0; i<N;i++){
    //     for(int j=0;j<i; j++){
    //         int temp = mat2[i][j];
    //         mat2[i][j] = mat2[j][i];
    //         mat2[j][i] = temp;
    //     }
    // }


    //////////////////////////
    // Multiplying
    //////////////////////////

    for(int i=0;i<N;i++){

         #pragma acc data copy(mat1[i][0:N],mat2[0:N][0:N])
        {   
            #pragma acc parallel loop reduction(+:arr_sum) private(arr_sum) async
            for(int j=0;j<N;j++){
                arr_sum = 0;
                for(int k=0;k<N;k++){
                    arr_sum+= mat1[i][k] * mat2[k][j];
                }
                res[i][j] = arr_sum;
            }
            #pragma acc update self(res[i][0:N]) async

        }
    }
   
}

// gcc -fopenacc 1_d.c -o 1_d && time ./1_d