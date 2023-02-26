#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define NCOLS 800
#define NROWS 600
#define MAX_ITER 2000

void calculate_mandelbrot(int start_row, int end_row, int rank);
void save_image(unsigned char *image, const char *filename);

int main(int argc, char **argv) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int rows_per_process = NROWS / size;
    int start_row = rank * rows_per_process;
    int end_row = start_row + rows_per_process;

    unsigned char *image = (unsigned char *)malloc(NROWS * NCOLS * 3);

    calculate_mandelbrot(start_row, end_row, rank);

    MPI_Gather(image + start_row * NCOLS * 3, rows_per_process * NCOLS * 3, MPI_UNSIGNED_CHAR,
               image, rows_per_process * NCOLS * 3, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        save_image(image, "mandelbrot.ppm");
    }

    free(image);
    MPI_Finalize();
    return 0;
}

void calculate_mandelbrot(int start_row, int end_row, int rank) {
    double x, y, x0, y0, xtemp;
    int i, j, k, iter;
    unsigned char *image = (unsigned char *)malloc((end_row - start_row) * NCOLS * 3);

    for (i = start_row; i < end_row; i++) {
        for (j = 0; j < NCOLS; j++) {
            x0 = ((double)j - NCOLS / 2) * 4.0 / NCOLS;
            y0 = ((double)i - NROWS / 2) * 4.0 / NROWS;

            x = 0;
            y = 0;
            iter = 0;
            while (x * x + y * y < 4 && iter < MAX_ITER) {
                xtemp = x * x - y * y + x0;
                y = 2 * x * y + y0;
                x = xtemp;
                iter++;
            }

            k = (i - start_row) * NCOLS * 3 + j * 3;
            image[k] = (unsigned char)(iter % 256);
            image[k + 1] = (unsigned char)(iter % 256);
            image[k + 2] = (unsigned char)(iter % 256);
        }
    }

    MPI_Gather(image, (end_row - start_row) * NCOLS * 3, MPI_UNSIGNED_CHAR,
               NULL, (end_row - start_row) * NCOLS * 3, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);

    free(image);
}

void save_image(unsigned char *image, const char *filename) {
    FILE *fp;
    fp = fopen(filename, "wb");
    fprintf(fp, "P6\n%d %d\n255\n", NCOLS, NROWS);
    fwrite(image, sizeof(unsigned char), NROWS * NCOLS * 3, fp);
    fclose(fp);
}
