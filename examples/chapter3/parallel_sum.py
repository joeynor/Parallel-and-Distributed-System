import sys
from mpi4py import MPI


def block(n, processes, rank):
    """Return the start and length of a balanced block."""
    q, remainder = divmod(n, processes)
    count = q + (rank < remainder)
    start = rank * q + min(rank, remainder)
    return start, count


def main():
    comm = MPI.COMM_WORLD
    rank, size = comm.Get_rank(), comm.Get_size()
    try:
        if len(sys.argv) > 2:
            raise ValueError
        n = int(sys.argv[1]) if len(sys.argv) == 2 else 17
        if not 0 <= n <= 1_000_000:
            raise ValueError
    except ValueError:
        if rank == 0:
            print("Usage: parallel_sum.py [N], 0 <= N <= 1000000",
                  file=sys.stderr, flush=True)
        comm.Abort(1)
        return 1

    DATA, PARTIAL, RESULT = 40, 41, 42
    start, count = block(n, size, rank)
    if rank == 0:
        data = list(range(1, n + 1))
        serial = sum(data)
        local = data[start:start + count]
        for worker in range(1, size):
            offset, length = block(n, size, worker)
            comm.send(data[offset:offset + length],
                      dest=worker, tag=DATA)
        del data
    else:
        local = comm.recv(source=0, tag=DATA)
    # Empty lists are valid messages, and sum([]) is zero.
    assert len(local) == count
    partial = sum(local)
    if rank == 0:
        total = partial
        for worker in range(1, size):
            total += comm.recv(source=worker, tag=PARTIAL)
        formula = n * (n + 1) // 2
        failed = total != serial or total != formula
        label = "FAIL" if failed else "PASS"
        print(f"N={n} P={size} sum={total} serial={serial} "
              f"formula={formula} {label}", flush=True)
        for worker in range(1, size):
            comm.send(failed, dest=worker, tag=RESULT)
    else:
        comm.send(partial, dest=0, tag=PARTIAL)
        failed = comm.recv(source=0, tag=RESULT)
    return int(failed)


if __name__ == "__main__":
    raise SystemExit(main())
