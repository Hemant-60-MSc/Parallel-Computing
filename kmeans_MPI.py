from mpi4py import MPI
from math import sqrt, pow, ceil
import random
from sklearn.model_selection import train_test_split
import matplotlib.pyplot as plt
import argparse
import numpy as np
from numpy.linalg import norm
from collections import Counter

comm = MPI.COMM_WORLD
rank = comm.Get_rank()
numprocs = comm.Get_size()



####################################
# Global variables
####################################
dataset_path = "Dataset_Path_with_x_y_points"

parser = argparse.ArgumentParser()
parser.add_argument("--s", help="Number of clusters",
                    type=int, default=5)
parser.add_argument("--i", help="Number of clusters",
                    type=int, default=10)
args = parser.parse_args()

clusters = args.s
iterations = args.i
####################################
# Functions
####################################
def init_centers(data, clusters = 5):
    indices = random.sample(range(len(data)), clusters)
    return [data[i] for i in indices]
    # print(self.centers)

def calculate_centers(res): #res is list of sets
    centers = []
    for i in range(len(res)):
        x_sum, y_sum, size =0.0, 0.0, len(res[i])
        for point in res[i]:
            x, y= point
            x_sum+=x
            y_sum+=y
        
        centers.append((x_sum/size, y_sum/size))
    return centers

def euclidean_distance(p1, p2):
    x1, y1 = p1
    x2, y2 = p2
    return sqrt( pow(x2-x1, 2) + pow(y2-y1, 2))

def Assign_data_to_cluster(data, centers, clusters = 5):
    res = [[] for _ in range(clusters)]
    for point in data:
        min_index = 0
        # min_val = euclidean_distance(point, centers[0])
        for i in range(1, clusters):
            if euclidean_distance(point, centers[min_index]) > euclidean_distance(point, centers[i]):
                min_index = i
        
        res[min_index].append(point)
    return res

####################################
# Data loading
####################################
def read_data(path, sep = ","):
    data = []
    with open(path, "r+") as file:
        for line in file.read().split("\n")[1:]:
            # curr_tuple = tuple([float(i)  for i in line.split(sep) if i!=""])
            curr_tuple = [float(i)  for i in line.split(sep) if i!=""]

            if len(curr_tuple) >=2: data.append(curr_tuple)
    return data


####################################
# Initializing model
####################################
start = MPI.Wtime()
centers = []
data = []
n = None
nt = None
if rank == 0:
    data, test = train_test_split(read_data(dataset_path, ","), test_size=0.2)
    n = len(data)
    nt = len(test)

n= comm.bcast(n, root=0)
nt= comm.bcast(nt, root=0)

comm.barrier()

factor= ceil(n/ numprocs)
# factor_t= ceil(nt/ numprocs)
max_process = ceil(n/ factor)
upper = rank*factor
lower = min(((rank+1) *factor), n)


if rank == 0:
    centers = init_centers(data, clusters)
    for process in range(1, max_process):
        upper_t = process*factor
        lower_t = min(((process+1) *factor), n)
        comm.send(data[upper_t:lower_t], dest=process)
elif rank < max_process:
    data = comm.recv(source = 0)


####################################
# Training model
####################################
centers = comm.bcast(centers, root=0)
for it in range(iterations):
    
    if rank > 0:res = Assign_data_to_cluster(data, centers, clusters)
    if rank == 0: res = Assign_data_to_cluster(data[:lower], centers, clusters)
    
    comm.barrier()

    if 0<rank<max_process:
        comm.send(res, dest=0)
    elif rank ==0:

        #collecting res from all other processes
        for process in range(1, max_process):
            temp_res = comm.recv(source = process)
            for i in range(len(temp_res)):
                res[i] += temp_res[i]

        # calculating new centers based on collected res
        centers = [(sum([j[0] for j in i])/len(i) , sum([j[1] for j in i])/len(i)) for i in res]
        # print("Train: Iteration {}".format(it))
        # print(centers)
    centers = comm.bcast(centers, root=0)
    
    comm.barrier()

comm.barrier()

####################################
# Plotting clusters
####################################
# if rank ==0:
#     for cluster in res:
#         plt.scatter([i[0] for i in cluster], [i[1] for i in cluster])
#     plt.show()

####################################
# Testing model
####################################
def classify_data(centers, data):
    res = []
    for point in data:
        min_index = 0
        for i in range(1, len(centers)):
            if euclidean_distance(point, centers[min_index]) > euclidean_distance(point, centers[i]):
                min_index = i        
        res.append(min_index)
    return res

factor= ceil(nt/ numprocs)
max_process = ceil(nt/ factor)
upper = rank*factor
lower = min(((rank+1) *factor), nt)

#sending test data to different processes
if rank == 0:
    centers = init_centers(test, clusters)
    for process in range(1, max_process):
        upper_t = process*factor
        lower_t = min(((process+1) *factor), nt)
        comm.send(test[upper_t:lower_t], dest=process)
    test = test[:lower]
elif rank < max_process:
    test = comm.recv(source = 0)

#Classifying data 
res = classify_data(centers, test)

comm.barrier()

#Collecting classification results from all processes
if 0<rank<max_process:
    comm.send(res, dest=0)
elif rank == 0:
    for process in range(1, max_process):
        res_t = comm.recv(source = process)
        res = res + res_t
    # print("Predicted samples from each class", Counter(res))
end = MPI.Wtime()

if rank == 0: print(end-start)

MPI.Finalize()


