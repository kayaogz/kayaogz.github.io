#include <cstdio>
#include <algorithm>
#include <mpi.h>
#include <omp.h>
#include <immintrin.h>

using namespace std;

/**
  * A compiler avec mpic++ inversion.cpp -o inversion -O3 -fopenmp -mavx2
  * A executer avec mpirun -np num-procs ./inversion taille-de-tableau
  * num-procs != 1 execute la fonction inversionMPI
  * num-procs == 1 execute la fonction inversionAVX
  */

/**
  * NE PAS MODIFIER
  * Cette fonction rend l'inverse d'un __m256
  */
__m256 _mm256_invert_ps(__m256 vec)
{
  static __m256i perm = _mm256_set_epi32(0, 1, 2, 3, 4, 5, 6, 7);
  return _mm256_permutevar8x32_ps(vec, perm);
}

/**
  * Renverser les elements du tableau A alloue de taille size dans le processus 0.
  * Les autres processus appellent la meme fonction mais ont les valeurs initiales A = NULL et size = -1 (a communiquer)
  * Au retour, le tableau A du processus 0 sera surecrit par son inverse
  */
void inversionMPI(float *A, int size)
{
  // A IMPLANTER ...
}

/** 
  * A compiler avec mpic++ inversion.cpp -o inversion -O3 -fopenmp -mavx2
  * A executer avec mpirun -np 1 ./inversion [taille-de-tableau]
  */
void inversionAVX(float *A, int size)
{
  // A IMPLANTER ...
}

void printUsage()
{
  cout << "Utilisation:\n  mpirun -np num-procs ./inversion taille-du-tableau" << endl;
}

/**
  * NE PAS MODIFIER
  */
int main(int argc, char **argv)
{
  MPI_Init(&argc, &argv);
  int procRank, numProc;
  float *A = NULL, size = -1;
  MPI_Comm_rank(MPI_COMM_WORLD, &procRank);
  MPI_Comm_size(MPI_COMM_WORLD, &numProc); 
  if (argc < 2) {
    if (procRank == 0) printUsage();
    MPI_Finalize();
    return 0;
  }

  if (procRank == 0) {
    size = atoi(argv[1]);
    A = (float *) _mm_malloc (size * sizeof(float), 32);
    for (int i = 0; i < size; i++) { A[i] = size - i; }
  }

  if (numProc > 1) {
    if (procRank == 0) { cout << "Execution de inversionMPI avec " << numProc << " processus.\n"; }
    inversionMPI(A, size);
  } else {
    cout << "Execution de inversionAVX.\n";
    inversionAVX(A, size);
  }

  if (procRank == 0) {
    int i;
    printf("Les premiers 10 elements apres inversion: ");
    for (i = 0; i < 10; i++) { cout << A[i] << " "; }
    cout << endl;
    for (i = 0; i < size; i++) {
      if (A[i] != i + 1) { 
        printf("L'element %d du tableau est incorrect apres inversion.\n", i);
        break;
      }
    }
    if (i == size) { printf("L'inversion est effectue avec succes.\n"); }
    free(A);
  }

  MPI_Finalize();
  return 0;
}
