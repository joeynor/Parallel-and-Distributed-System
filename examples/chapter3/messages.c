#include <mpi.h>
#include <stdio.h>

int main(int argc, char **argv) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    if (size != 2) {
        if (rank == 0) fprintf(stderr, "Run with exactly 2 ranks\n");
        MPI_Finalize();
        return 1;
    }
    if (rank == 0) {
        double data[3] = {1.5, 2.5, 3.5};
        char text[] = "MPI";
        MPI_Send(data, 3, MPI_DOUBLE, 1, 10, MPI_COMM_WORLD);
        MPI_Send(text, (int)sizeof text, MPI_CHAR,
                 1, 11, MPI_COMM_WORLD);
    } else {
        double data[8];
        char text[16];
        int received;
        MPI_Status status;
        MPI_Recv(data, 8, MPI_DOUBLE, 0, 10,
                 MPI_COMM_WORLD, &status);
        MPI_Get_count(&status, MPI_DOUBLE, &received);
        printf("Source %d, tag %d, count %d:",
               status.MPI_SOURCE, status.MPI_TAG, received);
        for (int i = 0; i < received; ++i)
            printf(" %.1f", data[i]);
        printf("\n");
        MPI_Recv(text, 16, MPI_CHAR, 0, 11,
                 MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Text: %s\n", text);
    }
    MPI_Finalize();
    return 0;
}
