#include <mpi.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

/* Balanced contiguous blocks, with half-open ranges. */
static void block(int n, int p, int r, int *start, int *count) {
    int q = n / p, remainder = n % p;
    *count = q + (r < remainder);
    *start = r * q + (r < remainder ? r : remainder);
}

int main(int argc, char **argv) {
    int rank, size, n = 17, start, count;
    const int DATA = 40, PARTIAL = 41, RESULT = 42;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    if (argc > 2) {
        if (rank == 0) fprintf(stderr, "Usage: parallel_sum [N]\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
        return 1;
    }
    if (argc == 2) {
        char *end;
        errno = 0;
        long parsed = strtol(argv[1], &end, 10);
        if (errno || end == argv[1] || *end != '\0' ||
            parsed < 0 || parsed > 1000000) {
            if (rank == 0) fprintf(stderr, "N must be 0..1000000\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
            return 1;
        }
        n = (int)parsed;
    }
    block(n, size, rank, &start, &count);
    /* Allocate a valid buffer even when count is zero. */
    int *local = malloc((size_t)(count ? count : 1) * sizeof *local);
    if (!local) {
        MPI_Abort(MPI_COMM_WORLD, 2);
        return 2;
    }
    long long serial = 0;
    if (rank == 0) {
        int *all = malloc((size_t)(n ? n : 1) * sizeof *all);
        if (!all) {
            MPI_Abort(MPI_COMM_WORLD, 2);
            return 2;
        }
        for (int i = 0; i < n; ++i) {
            all[i] = i + 1;
            serial += all[i];
        }
        for (int i = 0; i < count; ++i) local[i] = all[i];
        for (int r = 1; r < size; ++r) {
            int offset, length;
            block(n, size, r, &offset, &length);
            MPI_Send(all + offset, length, MPI_INT,
                     r, DATA, MPI_COMM_WORLD);
        }
        free(all);
    } else {
        MPI_Recv(local, count, MPI_INT, 0, DATA,
                 MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    long long partial = 0;
    for (int i = 0; i < count; ++i) partial += local[i];
    free(local);
    int failed = 0;
    if (rank == 0) {
        long long total = partial;
        for (int r = 1; r < size; ++r) {
            long long value;
            MPI_Recv(&value, 1, MPI_LONG_LONG_INT, r, PARTIAL,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            total += value;
        }
        long long formula = (long long)n * (n + 1) / 2;
        failed = (total != serial || total != formula);
        printf("N=%d P=%d sum=%lld serial=%lld formula=%lld %s\n",
               n, size, total, serial, formula,
               failed ? "FAIL" : "PASS");
        for (int r = 1; r < size; ++r)
            MPI_Send(&failed, 1, MPI_INT, r, RESULT, MPI_COMM_WORLD);
    } else {
        MPI_Send(&partial, 1, MPI_LONG_LONG_INT, 0, PARTIAL,
                 MPI_COMM_WORLD);
        MPI_Recv(&failed, 1, MPI_INT, 0, RESULT,
                 MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    MPI_Finalize();
    return failed;
}
