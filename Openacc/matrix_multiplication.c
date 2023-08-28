/*

Parallel directive for initialization and multiplying

*/

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

    int **mat1 = (int **)malloc(N * sizeof(int *));
    int **mat2 = (int **)malloc(N * sizeof(int *));
    int **res = (int **)malloc(N * sizeof(int *));
    
    for (int r = 0; r < N; r++) {
        mat1[r] = (int *)malloc(N * sizeof(int));
        mat2[r] = (int *)malloc(N * sizeof(int));
        res[r] = (int *)malloc(N * sizeof(int));
    }

    #pragma acc parallel loop
    for(int i=0;i<N;i++){

        // #pragma acc parallel loop
        for(int j =0; j<N;j++){
            mat1[i][j] = rand()%10;
            mat2[i][j] = rand()%10;
        }
    }

    //////////////////////////////
    // Printing matrices
    //////////////////////////////
    // printf("############# Matrix:1 #############\n");
    // print_mat(mat1, N);
    // printf("######################################\n");
    // printf("############# Matrix:2 #############\n");
    // print_mat(mat2, N);
    // printf("######################################\n");

    //////////////////////////////
    // Multiplying Matrices
    //////////////////////////////
     //reduction(+: arr_sum)
    for(int i=0;i<N;i++){

        // #pragma acc parallel loop
        #pragma acc parallel loop reduction(+:arr_sum) private(arr_sum)
        for(int j=0;j<N;j++){
            arr_sum = 0;
            
            for(int k =0; k< N;k++){

                // #pragma acc atomic update
                arr_sum += mat1[i][k] * mat2[k][j];
            }
            res[i][j] = arr_sum;
        }
    }

    //////////////////////////////
    // Printing results
    //////////////////////////////
    printf("############# Matrix:res #############\n");
    print_mat(res, N);
    printf("######################################\n");
}

// gcc -fopenacc 1_b.c -o 1_b && time ./1_b