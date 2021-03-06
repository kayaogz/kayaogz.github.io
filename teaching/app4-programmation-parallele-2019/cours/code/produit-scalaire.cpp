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
  MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);
//  if (rank == 0) { // J'envois a tout le monde
//    MPI_Request *req = (MPI_Request *) malloc (size * sizeof(MPI_Request));
//    for (int i = 1; i < size; i++) {
//      MPI_Isend(&N, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &req[i]);
//    }
////    for (int i = 1; i < size; i++) { MPI_Wait(&req[i], MPI_STATUS_IGNORE); }
//    MPI_Waitall(size - 1, &req[1], MPI_STATUSES_IGNORE);
//  } else { // Je recois du processus 0
//    MPI_Recv(&N, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
//    std::cout << rank << " " << N << std::endl;
//  }

  // Allouer a_local et b_local dans chaque processus, puis envoyer la partie correspondante du a_global et b_global du 
  // processus 0 a chaque processus
  a_local = (double *) malloc (sizeof(double) * N / size);
  b_local = (double *) malloc (sizeof(double) * N / size);

  MPI_Scatter(&a_global[0], N / size, MPI_DOUBLE, &a_local[0], N / size, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  MPI_Scatter(&b_global[0], N / size, MPI_DOUBLE, &b_local[0], N / size, MPI_DOUBLE, 0, MPI_COMM_WORLD);

//  if (rank == 0) { // Envoyer les parties correspondantes de a_global et b_global aux processus
//    for (int i = 1; i < size; i++) {
//      MPI_Send(&a_global[i * N / size], N / size, MPI_DOUBLE, i, 0, MPI_COMM_WORLD);
//      MPI_Send(&b_global[i * N / size], N / size, MPI_DOUBLE, i, 1, MPI_COMM_WORLD);
//    }
//    for (int i = 0; i < N / size; i++) { 
//      a_local[i] = a_global[i];
//      b_local[i] = b_global[i];
//    }
//  } else { // Recevoir les parties correspondantes de a_global et b_global dans chaque processus
//    MPI_Recv(&a_local[0], N / size, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
//    MPI_Recv(&b_local[0], N / size, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
//  }

  // Calculer le produit scalaire local dans chaque processus, puis envoyer le resultat au processus 0 pour sommer
  // Afficher le resultat du processus 0
  double somme = 0.0;
  for (int i = 0; i < N / size; i++) { somme += a_local[i] * b_local[i]; }

  double *rbuf;
  if (rank == 0) { rbuf = (double *) malloc(size * sizeof(double)); }
  MPI_Gather(&somme, 1, MPI_DOUBLE, rbuf, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  if (rank == 0) {
    for (int i = 1; i < size; i++) { somme += rbuf[i]; }
    std::cout << "Produit scalaire: " << somme;
  }
  if (rank == 0) { free(rbuf); }

//  if (rank == 0) { // Recevoir la somme partielle de tous les processus
//    for (int i = 1; i < size; i++) {
//      double temp;
//      MPI_Recv(&temp, 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
//      somme += temp;
//    }
//    std::cout << "Produit scalaire: " << somme;
//  } else { // Envoyer la somme partielle au processus 0
//    MPI_Send(&somme, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
//  }

  // Desallouer a_local et b_local dans chaque processus
  free(a_local);
  free(b_local);

  // Desallouer a_global et b_global
  if (rank == 0) {
    free(a_global);
    free(b_global);
  }

  MPI_Finalize();
  return 0;
}
