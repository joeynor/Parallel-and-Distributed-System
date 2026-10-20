from mpi4py import MPI

comm = MPI.COMM_WORLD
rank = comm.Get_rank()
size = comm.Get_size()
host = MPI.Get_processor_name()
print(f"Rank {rank} of {size} on {host}", flush=True)
if rank == 0:
    print(f"Launched {size} processes", flush=True)
