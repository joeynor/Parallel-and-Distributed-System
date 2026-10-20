#include <mpi.h>
#include <stdio.h>

int main(int argc, char **argv) {
    int rank, size, name_length;
    char host[MPI_MAX_PROCESSOR_NAME];
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Get_processor_name(host, &name_length);
    printf("Rank %d of %d on %s\n", rank, size, host);
    if (rank == 0) printf("Launched %d processes\n", size);
    MPI_Finalize();
    return 0;
}
