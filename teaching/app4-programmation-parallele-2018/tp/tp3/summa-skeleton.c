#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "mpi.h"

#define CREATE_MATRIX_FILL_ZERO 0
#define CREATE_MATRIX_FILL_RANDOM 1
#define CREATE_MATRIX_FILL_NONE 2

double *createMatrix(
    int size,
    int fill)
{
  double *mat;
  int i;
  mat = (double *)malloc(size * size * sizeof(mat[0]));
  if (fill == CREATE_MATRIX_FILL_RANDOM) { // Initialize the matrix elements randomly
    for (i = 0; i < size * size; i++) { mat[i] = rand() / (double)RAND_MAX; }
  } else if (fill == CREATE_MATRIX_FILL_ZERO) { // Set all matrix elements to zero
    memset(mat, 0, size * size * sizeof(mat[0]));
  } else if (fill == CREATE_MATRIX_FILL_NONE) { // Do not fill anything
  }
  else { printf("ERROR: Unknown matrix fill type.\n"); }
  return mat;
}

/** Multiply two matrices A and B, and add the result to another matrix C.
 */
void multiplyMatrix(
    double *A,
    double *B,
    double *C,
    int size)
{
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      for (int k = 0; k < size; k++) {
        C[i + j * size] += A[i + k * size] * B[k + j * size];
      }
    }
  }
}

void printMatrix(
    double *mat,
    int size)
{
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      printf("%12lf", mat[i + j * size]);
    }
    printf("\n");
  }
}

void printUsage()
{
  printf("Usage: mpirun -np [num-procs] ./summa [matrix-size]\n");
}

/** Convert the matrix A in column-major layout to matrix Ab in blocked layout. The matrix Ab is decomposed into
 * (numBlocks x numBlocks) blocks.
 **/
void convertNormalToBlocked(
    double * const A,
    double *Ab,
    int size,
    int numBlocks)
{
  const int blockSize = size / numBlocks;
  double *curBlockElem = Ab;
  for (int bj = 0; bj < numBlocks; bj++) {
    for (int bi = 0; bi < numBlocks; bi++) {
      double *blockBegin = A + bi * blockSize + bj * blockSize * size;
      for (int j = 0; j < blockSize; j++) {
        for (int i = 0; i < blockSize; i++) {
          (*curBlockElem) = blockBegin[i + j * size];
          curBlockElem++;
        }
      }
    }
  }
}

/** Determine if two matrices A and B of size (size x size) is equal.
  */
int matEqual(
    double * const A,
    double * const B,
    int size)
{
  double normA = 0.0, normB = 0.0, normDiff = 0.0;
  for (int j = 0; j < size; j++) {
    for (int i = 0; i < size; i++) {
      double aij = A[i + j * size];
      double bij = B[i + j * size];
      double diff = aij - bij;
      normA += aij * aij;
      normB += bij * bij;
      normDiff += diff * diff;
    }
  }
  double maxNorm = (fabs(normA) > fabs(normB) ? fabs(normA) : fabs(normB));
  if (maxNorm < 1e-12) { return (normDiff < 1e-12); }
  else { return ((normDiff / maxNorm) < 1e-6); }
}

/** Convert the matrix Ab in blocked format to the matrix A in non-blocked column-major layout. The matrix Ab is
 * decomposed into (numBlocks x numBlocks) blocks.
 **/
void convertBlockedToNormal(
    double * const Ab,
    double *A,
    int size,
    int numBlocks)
{
  const int blockSize = size / numBlocks;
  double *curBlockElem = Ab;
  for (int bj = 0; bj < numBlocks; bj++) {
    for (int bi = 0; bi < numBlocks; bi++) {
      double *blockBegin = A + bi * blockSize + bj * blockSize * size;
      for (int j = 0; j < blockSize; j++) {
        for (int i = 0; i < blockSize; i++) {
          blockBegin[i + j * size] = (*curBlockElem);
          curBlockElem++;
        }
      }
    }
  }
}

/* Parallel SUMMA matrix-matrix multiplication of two matrices of size NxN
 */
void summa(
    double *Aloc,
    double *Bloc,
    double *Cloc,
    int N)
{
    int rank, P, p;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &P);
    p = (int)sqrt(P);

    /* Create communicators */
    int row_color = rank % p;
    MPI_Comm rowComm;
    MPI_Comm_split(MPI_COMM_WORLD, row_color, rank, &rowComm);
    int rowRank;
    MPI_Comm_rank(rowComm, &rowRank);

    int col_color = rank / p;
    MPI_Comm colComm;
    MPI_Comm_split(MPI_COMM_WORLD, col_color, rank, &colComm);
    int colRank;
    MPI_Comm_rank(colComm, &colRank);

    double *Atemp, *Btemp;
    Atemp = createMatrix(N / p, CREATE_MATRIX_FILL_NONE);
    Btemp = createMatrix(N / p, CREATE_MATRIX_FILL_NONE);

    int blockSize = (N / p) * (N / p);

    /* Compute */
    for (int k = 0; k < p; ++k) {
      if (rowRank == k)
        memcpy(Atemp, Aloc, blockSize * sizeof(Atemp[0]));
      MPI_Bcast(Atemp, blockSize, MPI_DOUBLE, k, rowComm);
      if (colRank == k)
        memcpy(Btemp, Bloc, blockSize * sizeof(Btemp[0]));
      MPI_Bcast(Btemp, blockSize, MPI_DOUBLE, k, colComm);
      multiplyMatrix(Atemp, Btemp, Cloc, N / p);
    }

    /* Cleanup */
    MPI_Comm_free(&rowComm);
    MPI_Comm_free(&colComm);
    free(Atemp);
    free(Btemp);
}

void summaFull(int N)
{
  int rank, P;
  double *A, *B, *C, *Ccopy, *Ab, *Bb, *Cb;
  double *Aloc, *Bloc, *Cloc;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &P);
  int p = (int)sqrt(P);
  // Create the matrix A in the root processor
  if (rank == 0) {
    A = createMatrix(N, CREATE_MATRIX_FILL_RANDOM);
    B = createMatrix(N, CREATE_MATRIX_FILL_RANDOM);
    C = createMatrix(N, CREATE_MATRIX_FILL_RANDOM);
    Ccopy = createMatrix(N, CREATE_MATRIX_FILL_NONE);
    memcpy(Ccopy, C, N * N * sizeof(Ccopy[0]));
    printf("Matrix A:\n"); printMatrix(A, N); printf("\n");
    printf("Matrix B:\n"); printMatrix(B, N); printf("\n");
    printf("Matrix C:\n"); printMatrix(C, N); printf("\n");
  }
  // (REF)Convert matrices from normal to blocked layout
  if (rank == 0) {
    Ab = (double *)malloc(N * N * sizeof(double));
    convertNormalToBlocked(A, Ab, N, p);
    Bb = (double *)malloc(N * N * sizeof(double));
    convertNormalToBlocked(B, Bb, N, p);
    Cb = (double *)malloc(N * N * sizeof(double));
    convertNormalToBlocked(C, Cb, N, p);
  }
  // (REF)Scatter the matrices A, B, and C
  Aloc = createMatrix(N / p, CREATE_MATRIX_FILL_NONE);
  MPI_Scatter(Ab, (N / p) * (N / p), MPI_DOUBLE, Aloc, (N / p) * (N / p), MPI_DOUBLE, 0, MPI_COMM_WORLD);
  Bloc = createMatrix(N / p, CREATE_MATRIX_FILL_NONE);
  MPI_Scatter(Bb, (N / p) * (N / p), MPI_DOUBLE, Bloc, (N / p) * (N / p), MPI_DOUBLE, 0, MPI_COMM_WORLD);
  Cloc = createMatrix(N / p, CREATE_MATRIX_FILL_NONE);
  MPI_Scatter(Cb, (N / p) * (N / p), MPI_DOUBLE, Cloc, (N / p) * (N / p), MPI_DOUBLE, 0, MPI_COMM_WORLD);
  // (REF)Call SUMMA algorithm
  summa(Aloc, Bloc, Cloc, N);
  
  // (REF)Gather the matrix C
  MPI_Gather(Cloc, (N / p) * (N / p), MPI_DOUBLE, Cb, (N / p) * (N / p), MPI_DOUBLE, 0, MPI_COMM_WORLD);
  if (rank == 0) {
    // (REF)Convert blocked layout to normal layout
    convertBlockedToNormal(Cb, C, N, p);
  }

  // Multiply normal matrices sequentially in the root processor, then compare the results.
  if (rank == 0) {
    multiplyMatrix(A, B, Ccopy, N);
    printf("Computed result:\n"); printMatrix(C, N); printf("\n");
    printf("Actual result:\n"); printMatrix(Ccopy, N); printf("\n");
    if (matEqual(C, Ccopy, N)) { printf("The multiplication is correct, bravo!\n"); }
    else { printf("Bad news... The multiplication is incorrect.\n"); }
    free(A);
    free(B);
    free(C);
  }
}

int main(int argc, char **argv) {
  MPI_Init(&argc, &argv);
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  if (argc < 2) {
    if (rank == 0)  { printUsage(); }
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Abort(MPI_COMM_WORLD, 1);
  }
  srand(123);
  int N = atoi(argv[1]);
  summaFull(N);
  MPI_Finalize();
  return 0;
}
