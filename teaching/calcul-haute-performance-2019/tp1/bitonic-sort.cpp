#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>
#include "mpi.h"

int MPI_ScatterSingleInt(
  const int *sendbuf,
  int *recvbuf,
  int root,
  MPI_Comm comm)
{
  // ...
  
  return MPI_SUCCESS;
}

int MPI_GatherSingleInt(
  const int *sendbuf,
  int *recvbuf,
  int root,
  MPI_Comm comm)
{
  // ...
  
  return MPI_SUCCESS;
}

void bitonicSort(
    int *arr,       // The array of elements to sort. It exists only in the master rank, and is NULL in other ranks.
    int N)          // The number of elements and MPI processes
{
  // Obtain the rank of the process
  // ...

  // First scatter the elements of arr in the master process to others so that each process has its element in procElem.
  // ...
  int procElem;

  // Then perform the bitonic separator for logN steps
  // ...

  // Finally, gather the final results in procElem back in the arr array of the master process
  // ...
}

void readArray(
    std::vector<int> &arr,
    int N,
    char *fileName)
{
  arr.resize(N);
  std::ifstream file(fileName, std::ifstream::in);
  if (!file) { 
    std::cout << "ERROR: Unable to open the file " << fileName << ".\n";
    MPI_Abort(MPI_COMM_WORLD, 1);
  }
  for (int i = 0; i < N; i++) { file >> arr[i]; }
  file.close();
}

void printArray(
    int *arr,
    int N)
{
  for (int i = 0; i < N; i++) { std::cout << arr[i] << " "; }
  std::cout << std::endl;
}

int checkArrSorted(
    int *arr,
    int N)
{
  for (int i = 0; i < N - 1; i++) {
    if (arr[i] > arr[i + 1]) { return 0; }
  }
  return 1;
}

void printUsage()
{
  std::cout << "Usage: mpirun -np [num-procs/elements] ./bitonic-sort [bitonic-array-file] [sequential|parallel]\n";
  std::cout << "  Example: mpirun -np 8 ./bitonic-sort bitonic-array.txt parallel\n";
}

int main(int argc, char **argv)
{
  std::vector<int> arr;
  int N;            // The size of arr. Equally, the number of available MPI ranks.
  int procRank;     // The rank of current process

  // Initialize MPI, get the process rank and the number of processes (which is also the size of arr)
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &procRank);
  MPI_Comm_size(MPI_COMM_WORLD, &N);

  if (argc < 3) { // Error check for command line parameters
    printUsage();
    return 1;
  }
  MPI_Barrier(MPI_COMM_WORLD);

  if (procRank == 0) { // Read the bitonic array from file in the root process
    readArray(arr, N, argv[1]);
    printf("->The array before bitonic sort:\n");
    printArray(&arr[0], N);
  }

  if (strcmp(argv[2], "sequential") == 0) { // Perform a sequential sort
    if (procRank == 0) {
      std::sort(arr.begin(), arr.end());
    }
  } else if (strcmp(argv[2], "parallel") == 0) { // Perform a parallel bitonic sort
    bitonicSort(&arr[0], N);
  } else {
    if (procRank == 0) {
      printf("ERROR: Unknown array sort type %s.\n", argv[2]);
    }
    return 1;
  }

  // Print the array and verify if it is sorted
  if (procRank == 0) {
    printf("->The array after bitonic sort:\n");
    printArray(&arr[0], N);
    if (checkArrSorted(&arr[0], N)) {
      printf("->The array is sorted correctly!\n");
    } else {
      printf("->The array does not seem to be sorted!\n");
    }
  }

  MPI_Finalize();
  return 0;
}
