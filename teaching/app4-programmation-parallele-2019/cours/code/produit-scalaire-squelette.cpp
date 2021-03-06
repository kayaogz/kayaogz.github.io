#include <iostream>
#include <cstdio>
#include "mpi.h"

int main(int argc, char **argv)
{
  double *a_global, *a_local;
  double *b_global, *b_local;
  int rank, size;
  int N;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  
  // Initialiser les tableaux a_global et b_global dans le processus 0
  if (rank == 0) {
    N = 32;
    a_global = (double *) malloc(N * sizeof(double));
    b_global = (double *) malloc(N * sizeof(double));
    for (int i = 0; i < N; i++) {
      a_global[i] = i;
      b_global[i] = 1;
    }
  }
  // Envoyer la taille de tableau a chaque processus. On suppose que N est divisible par le nombre de processus.
  // A FAIRE ...

  // Allouer a_local et b_local dans chaque processus, puis envoyer la partie correspondante du a_global et b_global du 
  // processus 0 a chaque processus
  // A FAIRE ...

  // Calculer le produit scalaire local dans chaque processus, puis envoyer le resultat au processus 0 pour sommer
  // Afficher le resultat du processus 0
  // A FAIRE ...

  // Desallouer a_local et b_local dans chaque processus
  // A FAIRE ...

  // Desallouer a_global et b_global
  if (rank == 0) {
    free(a_global);
    free(b_global);
  }

  MPI_Finalize();
  return 0;
}
