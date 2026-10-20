#include <mpi.h>
#include <stdio.h>

int main(int argc, char **argv) {
    int rank, size, token;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    if (size == 1) {
        printf("One rank: token remains 0\n");
    } else if (rank == 0) {
        token = 0;
        MPI_Send(&token, 1, MPI_INT, 1, 30, MPI_COMM_WORLD);
        MPI_Recv(&token, 1, MPI_INT, size - 1, 30,
                 MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Returned token: %d; expected: %d\n",
               token, size * (size - 1) / 2);
    } else {
        MPI_Recv(&token, 1, MPI_INT, rank - 1, 30,
                 MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        token += rank;
        MPI_Send(&token, 1, MPI_INT, (rank + 1) % size,
                 30, MPI_COMM_WORLD);
    }
    MPI_Finalize();
    return 0;
}
