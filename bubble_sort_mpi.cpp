// Passing whole array to all processes
// Comparing elementwise to get index in sorted array
// 
#include<iostream>
#include<vector>
#include<omp.h>
#include <stdio.h>      
#include <stdlib.h>     
#include <time.h>
#include <string.h>       
#include <mpi.h>
#include<math.h>
#include <limits.h>
#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define CEIL(A, B) A%B ==0? A/B: (A/B) + 1

using namespace std;

int read_int( int argc, char **argv, const char *option, int default_value );
int find_option( int argc, char **argv, const char *option );

void generate_random_num(vector<int>& nums){
    int num = 0;
    int limit = nums.size();
    #pragma omp parallel for num_threads(5) schedule(dynamic)
    for (int i =0; i <limit; i++){
        #pragma omp critical
        nums[i] = rand()%limit;
    }
}

void print_vector(vector<int> nums, int range){
    for (int i=0; i< range; i++)
        cout<<nums[i]<<" ";
    cout<<endl<<"==========================="<<endl;
}

void print_vector(vector<bool> nums, int range){
    range = nums.size();
    for (int i=0; i< range; i++)
        cout<<nums[i]<<" ";
    cout<<endl<<"==========================="<<endl;
}


void check_sorted(vector<int> nums){
    for (int i=0; i < nums.size()-1; i++){
        if (nums[i] > nums[i+1]){
            cout<<"Vector is not sorted"<<endl;
            return;
        }

    }
    cout<<"Vector is sorted"<<endl;
}


int get_index(vector<int> nums, int index){
    int count = 0;

    for(int i=0;i<nums.size(); i++){
        if (nums[i] < nums[index])
            ++count;
    }

    return count;
}




int main(int argc, char **argv){
    int numprocs, rank, upper, lower, factor, upper_t, lower_t, res_size;
    int n = read_int( argc, argv, "-s", 1000 );
    int threads = read_int( argc, argv, "-t", 4 );
    int i, j;

    MPI::Init();
    // MPI_Init(&argc, &argv);
    MPI_Comm_size (MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank (MPI_COMM_WORLD, &rank);
    cout<<numprocs<<endl;
    MPI_Status status;
    factor= CEIL(n, numprocs);
    int max_process = CEIL(n ,factor); // total number of processes

    upper = rank*factor;
    lower = MIN(((rank+1) *factor), n);

    MPI_Barrier(MPI_COMM_WORLD);
    omp_set_num_threads(threads);
    
    vector<int> nums(n);
    if (rank == 0){
        generate_random_num(nums);
        check_sorted(nums);

    }
  
    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Bcast( &nums[0], n, MPI_INT, 0, MPI_COMM_WORLD);
    
    MPI_Barrier(MPI_COMM_WORLD);

    vector<int> ind_res(lower-upper);

    for(i =0;i <(lower-upper); i++){
        ind_res[i] = get_index(nums, upper + i);
    }
    
    MPI_Barrier(MPI_COMM_WORLD);

    if (rank==0){
        vector<int> res(n, INT_MIN);

        for(i = 0; i<lower;i++){
            res[ind_res[i]] = nums[i];
        }

        for(int process = 1; process<max_process; process++){
            upper_t = process*factor;
            lower_t = MIN(((process+1) *factor), n);
            res_size = lower_t - upper_t;
            vector<int> res_t(res_size);
            MPI_Recv(&res_t[0], lower_t - upper_t, MPI_INT, process, 0, MPI_COMM_WORLD, &status);
            for(i = upper_t; i < lower_t; i++ ){
                res[res_t[i-upper_t]] = nums[i];
            }
        }
        for (i =1; i<n;i++){
            if(res[i] == INT_MIN){
                res[i] = res[i-1];
            }
        }
        check_sorted(res);


    }else if(rank<max_process){
        MPI_Send(&ind_res[0], lower-upper,MPI_INT,0, 0,MPI_COMM_WORLD);
    }




    MPI_Barrier(MPI_COMM_WORLD);

    MPI::Finalize();

    return 0;
}

int read_int( int argc, char **argv, const char *option, int default_value )
{
    int iplace = find_option( argc, argv, option );
    if( iplace >= 0 && iplace < argc-1 )
        return atoi( argv[iplace+1] );
    return default_value;
}

int find_option( int argc, char **argv, const char *option )
{
    for( int i = 1; i < argc; i++ )
        if( strcmp( argv[i], option ) == 0 )
            return i;
    return -1;
}
