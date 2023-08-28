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
#include<deque>
#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define CEIL(A, B) A%B ==0? A/B: (A/B) + 1
// #define ABS(A) A<0? -A: A

using namespace std;

int read_int( int argc, char **argv, const char *option, int default_value );
int find_option( int argc, char **argv, const char *option );

void generate_random_num(vector<int>& nums){
    int num = 0;
    int limit = nums.size();
    #pragma omp parallel for schedule(dynamic)
    for (int i =0; i <limit; i++){
        #pragma omp critical
        nums[i] = rand()%limit;
    }
}

void print_vector(vector<int> nums, int range){
    cout<<"Size "<<nums.size()<<endl;
    for (int i=0; i< nums.size(); i++)
        cout<<nums[i]<<" ";
    cout<<endl<<"==========================="<<endl;
}

void print_vector(deque<int> nums, int range){
    // cout<<"Size "<<nums.size()<<endl;
    for (int i=0; i< nums.size(); i++)
        cout<<nums[i]<<" ";
    cout<<endl<<"==========================="<<endl;
}

void print_vector(vector<bool> nums, int range){
    range = nums.size();
    for (int i=0; i< range; i++)
        cout<<nums[i]<<" ";
    cout<<endl<<"==========================="<<endl;
}

void bubble_sort(deque<int>& vals){
    int sz = vals.size();
    for(int i =0 ;i<sz;i++){
        for(int j = i+1;j<sz;j++){
            if(vals[i] > vals[j]){
                swap(vals[i], vals[j]);
            } 
        }
    }
}
void bubble_sort(vector<int>& vals){
    int sz = vals.size();
    for(int i =0 ;i<sz;i++){
        for(int j = i+1;j<sz;j++){
            if(vals[i] > vals[j]){
                swap(vals[i], vals[j]);
            } 
        }
    }
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
    double start = MPI_Wtime();
    int numprocs, rank, upper, lower, factor, upper_t, lower_t, res_size;
    int n = read_int( argc, argv, "-s", 1000 );
    int threads = read_int( argc, argv, "-t", 4 );
    int i, j;

    MPI_Init(&argc, &argv);
    MPI_Comm_size (MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank (MPI_COMM_WORLD, &rank);
    MPI_Status status;
    factor= CEIL(n, numprocs);
    int max_process = CEIL(n ,factor); // total number of processes
    upper = rank*factor;
    lower = MIN(((rank+1) *factor), n);
    // cout<<lower<<" "<<upper<<endl;
    // deque<int> vals;
    vector<int> vals;
    // if (rank < max_process) deque<int> vals(lower-upper);
    // else deque<int> vals(factor);
    // deque<int> vals;

    MPI_Barrier(MPI_COMM_WORLD);
    omp_set_num_threads(threads);
    
    if (rank == 0){
        vector<int> nums(n);
        generate_random_num(nums);
        // check_sorted(nums);
        for(i =0; i< lower; i++) {
            // vals[i] = nums[i];
            vals.push_back(nums[i]);
        }
        for(int process =1; process< max_process; process++){
            upper_t = process*factor;
            lower_t = MIN(((process+1) *factor), n);
            vector<int> vals_t(lower_t-upper_t);
            for(int index = upper_t; index < lower_t; index++){
                vals_t[index-upper_t] = nums[index];
            }
            MPI_Send(&vals_t[0], lower_t-upper_t,MPI_INT,process, 0,MPI_COMM_WORLD);
        }
    }
    else if(rank<max_process){
        vals.resize(lower - upper);
        MPI_Recv(&vals[0], lower - upper, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
    }
    MPI_Barrier(MPI_COMM_WORLD);

    bubble_sort(vals);
    
    MPI_Barrier(MPI_COMM_WORLD);
    // print_vector(vals, 10);

    if (rank == 0){
        vector<deque<int>> collection(max_process);
        for(auto i: vals){
            collection[0].push_back(i);
        }
        for(int process =1; process< max_process; process++){
            upper_t = process*factor;
            lower_t = MIN(((process+1) *factor), n);
            vector<int> temp(lower_t-upper_t);
            MPI_Recv(&temp[0], lower_t-upper_t,MPI_INT,process, 0,MPI_COMM_WORLD, &status);
            // collection.push_back(temp);
            for(auto i: temp){
                collection[process].push_back(i);
            }
        }
        vector<int> res;
        for(i =0;i<n;i++){
            int min_index = -1;
            for(j =0;j<collection.size(); j++){
                if((min_index == -1 and !collection[j].empty()) or (!collection[j].empty() > 0 && collection[j][0] < collection[min_index][0])){
                    min_index = j;
                }
            }

            res.push_back(collection[min_index][0]);
            collection[min_index].pop_front();
        }

        // check_sorted(res);
 

    }
    else if(rank<max_process){
        MPI_Send(&vals[0], lower - upper, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    
    double end = MPI_Wtime();
    if(rank == 0) cout<<end-start<<endl;
    MPI_Finalize();

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
