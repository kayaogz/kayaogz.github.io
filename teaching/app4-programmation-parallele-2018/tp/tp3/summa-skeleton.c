#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "mpi.h"

#include "summa-solution.c"

void createMatrix(
    double **pmat,
    int nrows,
    int ncols,
    char *init)
{
  double *mat;
  int i;
  *pmat = mat = (double *)malloc(nrows * ncols * sizeof(mat[0]));
  if (strcmp(init, "random") == 0) { // Initialize the matrix elements randomly
    srand(time(0));
    for (i = 0; i < nrows * ncols; i++) { mat[i] = rand() / (double)RAND_MAX; }
  } else { // Set all matrix elements to zero
    memset(mat, 0, nrows * ncols * sizeof(mat[0]));
  }
}

/** Multiply two matrices a(of size mxk) and b(of size kxn), and add the result to c(of size mxn)
 */
void multiplyMatrix(
    double *a,
    double *b,
    double *c,
    int M)
{
  int im, ik, in;
  for (im = 0; im < M; im++) {
    for (in = 0; in < M; in++) {
      for (ik = 0; ik < M; ik++) {
        c[im + in * M] += a[im + ik * M] * b[ik + in * M];
      }
    }
  }
}

void printUsage()
{
  printf("Usage: mpirun -np [num-procs] ./summa M\n");
}

int main(int argc, char **argv) {
  MPI_Init(&argc, &argv);

  int procRank;
  MPI_Comm_rank(MPI_COMM_WORLD, &procRank);
  if (argc < 2 && procRank == 0) {
    printUsage();
    MPI_Abort(MPI_COMM_WORLD, 1);
  }
  MPI_Barrier(MPI_COMM_WORLD);

  int M = atoi(argv[1]);
  double *A, *B, *C;
  createMatrix(&A, M, M, "random");
  createMatrix(&B, M, M, "random");
  createMatrix(&C, M, M, "zero");
  summa(A, B, C, M);

  MPI_Finalize();
  return 0;
}
