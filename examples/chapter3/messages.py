from mpi4py import MPI
import numpy as np

comm = MPI.COMM_WORLD
rank = comm.Get_rank()
if comm.Get_size() != 2:
    if rank == 0:
        print("Run with exactly 2 ranks", flush=True)
    raise SystemExit(1)

if rank == 0:
    # Upper-case methods transfer numerical buffers directly.
    data = np.array([1.5, 2.5, 3.5], dtype=np.float64)
    comm.Send([data, MPI.DOUBLE], dest=1, tag=10)
    # Lower-case methods serialize Python objects.
    comm.send({"text": "MPI", "values": [1, 2, 3]},
              dest=1, tag=11)
else:
    data = np.empty(8, dtype=np.float64)
    status = MPI.Status()
    comm.Recv([data, MPI.DOUBLE], source=0, tag=10,
              status=status)
    received = status.Get_count(MPI.DOUBLE)
    print(f"Source {status.Get_source()}, "
          f"tag {status.Get_tag()}, count {received}: "
          f"{data[:received].tolist()}", flush=True)
    payload = comm.recv(source=0, tag=11)
    print(f"Object: {payload}", flush=True)
