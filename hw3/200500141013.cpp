#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

void first_vector(double *vector, int size) {
    for (int i = 0; i < size; i++) {
        vector[i] = ((double) rand() / (double) RAND_MAX);
    }
}

void first_matrix(double *matrix, int size) {
    for (int i = 0; i < size*size; i++) {
        matrix[i] = ((double) rand() / (double) RAND_MAX);
    }
}


void multiply_vector(double *matrix, double *vector, double *result, int size) {
    for (int i = 0; i < size; i++) {
        result[i] = 0.0;
        for (int j = 0; j < size; j++) {
            result[i] += matrix[i * size + j] * vector[j];
        }
    }
}

void print_vector(double *vector, int size) {
    for (int i = 0; i < size; i++) {
        printf("%f ", vector[i]);
    }
    printf("\n");
}

int main(int argc, char *argv[]) {
    int id;
    int num_processes;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &num_processes);

    srand(19050111030 + id);

    int large_size = id % 10000;
    int small_size = 100 + (id % 100);

    double *large_matrix = malloc(large_size * large_size * sizeof(double));
    double *small_matrix = malloc(small_size * small_size * sizeof(double));
    double *vector = malloc(large_size * sizeof(double));
    double *result = malloc(large_size * sizeof(double));
    double *gathered_result = NULL;

    if (id == 0) {
        gathered_result = malloc(large_size * sizeof(double));
    }

    first_matrix(large_matrix, large_size);
    first_matrix(small_matrix, small_size);
    first_vector(vector, large_size);

    double start_time = MPI_Wtime();

    MPI_Bcast(vector, large_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    multiply_vector(large_matrix, vector, result, large_size);

    MPI_Gather(result, large_size, MPI_DOUBLE, gathered_result, large_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    double elapsed_time = MPI_Wtime() - start_time;

    if (id == 0) {
        printf("Elapsed time is %f seconds for parallel mxv with %d processes \n", elapsed_time, num_processes);
    }

    free(large_matrix);
    free(small_matrix);
    free(vector);
    free(result);
    if (gathered_result != NULL) {
        free(gathered_result);
    }

    MPI_Finalize();
    return 0;
}
