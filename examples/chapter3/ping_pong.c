#include <mpi.h>
#include <stdio.h>

int main(int argc, char **argv) {
    int rank, size, token = 0;
    const int warmup = 100, repetitions = 1000;
    double start = 0.0;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    if (size != 2) {
        if (rank == 0) fprintf(stderr, "Run with exactly 2 ranks\n");
        MPI_Finalize();
        return 1;
    }
    for (int i = -warmup; i < repetitions; ++i) {
        if (rank == 0) {
            if (i == 0) start = MPI_Wtime();
            MPI_Send(&token, 1, MPI_INT, 1, 20, MPI_COMM_WORLD);
            MPI_Recv(&token, 1, MPI_INT, 1, 21,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        } else {
            MPI_Recv(&token, 1, MPI_INT, 0, 20,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Send(&token, 1, MPI_INT, 0, 21, MPI_COMM_WORLD);
        }
    }
    if (rank == 0) {
        double elapsed = MPI_Wtime() - start;
        printf("Mean round trip: %.3f us\n",
               1.0e6 * elapsed / repetitions);
    }
    MPI_Finalize();
    return 0;
}
