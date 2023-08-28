import numpy as np
from mpi4py import MPI
from time import time
import matplotlib.pyplot as plt
import random
from math import e, exp


#############################
## Initializing variables
#############################

comm = MPI.COMM_WORLD
rank = comm.Get_rank()
numprocs = comm.Get_size()


num_len = size = 10_000_000

factor = num_len//numprocs

l = factor * rank
r = min((factor)*(rank+1), size)

X = [random.randint(-num_len,num_len)/num_len for _ in range(r-l)]
Y = [random.randint(-num_len,num_len)/num_len for _ in range(r-l)]
Z = [random.randint(num_len//e,num_len)/num_len for _ in range(r-l)]

#############################
## Integration functions
#############################

def f(x, y)->float:
    return exp(-0.5*(x*x + y*y))

def less_than_fx(l, r)->float:
    count = 0
    fx_sum_domain = 0
    for i in range(l, r):
        res = f(X[i], Y[i])
        if res > Z[i]:
            count+=1
            fx_sum_domain+=f(X[i], Y[i])
    return count, fx_sum_domain


def integrate(count, fx_s, low_limit, up_limit):
    frac_domain = count/num_len
    domain = float((up_limit-(low_limit)) ** 2) * frac_domain
    y_mean = fx_s/count
    return domain * y_mean


counts, sums = less_than_fx(0, len(X))
counts_np = np.array(counts, dtype="i")
sums_np = np.array(sums, dtype ="f")

comm.barrier()


#############################
## send/recv implementation
#############################

start = time()

if rank > 0:
    comm.send(counts,dest=0,tag=rank)
    comm.send(sums, dest = 0, tag = rank)

comm.barrier()

if rank == 0:
    for i in range(1, numprocs):
        counts+=comm.recv(source=i,tag=i)
        sums+=comm.recv(source = i, tag = i)
    # print("result from send/recv:", integrate(counts, sums, -1, 1))
    integrate(counts, sums, -1, 1)

comm.barrier()
end = time()
if rank == 0:
    print(end-start)
    # print("Time for send/recv:", end-start)

#############################
## Send/Recv implementation
#############################

start = time()

count = counts_np
sums = sums_np

if rank > 0:
    comm.Send(counts_np,dest=0,tag=rank)
    comm.Send(sums_np,dest=0,tag=rank)

comm.barrier()

if rank == 0:
    for i in range(1, numprocs):
        c_t = np.array(counts, dtype="i")
        s_t = np.array(sums, dtype ="f")
        comm.Recv(c_t, source=i,tag=i)
        comm.Recv(s_t, source=i,tag=i)
        count+=c_t
        sums+=s_t
        
    # print("result from Send/Recv:", integrate(counts, sums, -1, 1))
    integrate(counts, sums, -1, 1)

comm.barrier()
end = time()
if rank == 0:
    print(end-start)