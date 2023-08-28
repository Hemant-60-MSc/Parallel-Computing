import numpy as np
from mpi4py import MPI 
from time import time

comm = MPI.COMM_WORLD
rank = comm.Get_rank()
numprocs = comm.Get_size()
parents = numprocs//2 
# assert numprocs == 2

class sample:

    def __init__(self) -> None:
        self.data = []
        self.sum = 0

    def add_data(self, val = 0):
        self.data.append(val)
        self.sum+=val
    
    def pop(self):
        self.sum-=self.data.pop()

    def print_data(self):
        print("List:",self.data, " Sum:", self.sum)


def print_time(s, time):
    print("="*5, s, "="*5)
    print(time)
    print("="*15)


sample_obj = sample()


comm.barrier()

start = time()

if rank == 0:
    sample_obj.add_data(rank*10)
    sample_obj.add_data(rank*10 + 1)
    print("Value of object in Process {}".format(rank))
    sample_obj.print_data()
    comm.send(sample_obj,dest=1,tag=0)

comm.barrier()

end = time()

if rank ==0:print("time for sending: ",end-start)

comm.barrier()

start = time()

if rank == 1:
    sample_obj = comm.recv(source = 0, tag = 0)
    print("Value of object at process {} after receiving from process {}".format(rank, 0))
    sample_obj.print_data()

comm.barrier()

end= time()

if rank ==0:print("time for Receiving: ",end-start)

if rank ==1:
    print("changing data at process 1 by popping and adding 1313131")
    sample_obj.pop()
    sample_obj.add_data(1313131)
    sample_obj.print_data()
