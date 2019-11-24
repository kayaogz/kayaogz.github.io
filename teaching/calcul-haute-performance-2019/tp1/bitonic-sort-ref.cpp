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
  // Get the processor rank and the number of processes.
  int procRank; MPI_Comm_rank(comm, &procRank);
  int numProcs; MPI_Comm_size(comm, &numProcs);

  if (procRank == root) { // Scatter sendbuf to all processes from the root.
    for (int i = 0; i < numProcs; i++) {
      if (i == procRank) {
        recvbuf[0] = sendbuf[i]; // Directly copy the element for the root process.
      } else { // Send their elements to other processes.
        MPI_Send(sendbuf + i, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
      }
    }
  } else {
    MPI_Recv(recvbuf, 1, MPI_INT, root, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // Receive my element from the root.
  }
  
  return MPI_SUCCESS;
}

int MPI_GatherSingleInt(
  const int *sendbuf,
  int *recvbuf,
  int root,
  MPI_Comm comm)
{
  // Get the processor rank and the number of processes.
  int procRank; MPI_Comm_rank(comm, &procRank);
  int numProcs; MPI_Comm_size(comm, &numProcs);

  if (procRank == root) { // Scatter sendbuf to all processes.
    for (int i = 0; i < numProcs; i++) {
      if (i == procRank) {
        recvbuf[i] = sendbuf[0]; // Directly copy the element for the root process.
      } else {
        MPI_Recv(recvbuf + i, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // Receive each process's element.
      }
    }
  } else {
    MPI_Send(sendbuf, 1, MPI_INT, root, 0, MPI_COMM_WORLD); // Send my element to the root.
  }
  
  return MPI_SUCCESS;
}

void bitonicSort(
    int *arr,       // The array of elements to sort. It exists only in the master rank, and is NULL in other ranks.
    int N)          // The number of elements and MPI processes
{
  // Obtain the rank of the process
  int procRank; 
  MPI_Comm_rank(MPI_COMM_WORLD, &procRank);

  // First scatter the elements of arr in the master process to others so that each process has its element in procElem.
  int procElem;
  MPI_Scatter(arr, 1, MPI_INT, &procElem, 1, MPI_INT, 0, MPI_COMM_WORLD);
  // Do the same using the new scatter implementation
  // MPI_ScatterSingleInt(arr, &procElem, 0, MPI_COMM_WORLD);

  // Then perform the bitonic separator for logN steps
  int pairElem;
  int pairMask = N >> 1; // Flipping this bit should give my pair at each round

  while (pairMask > 0) {
    int pairRank = procRank ^ pairMask;
    // Solution using separate MPI_Send and MPI_Recv
//    if (pairRank < procRank) { 
//      MPI_Send(&procElem, 1, MPI_INT, pairRank, 0, MPI_COMM_WORLD);
//      MPI_Recv(&pairElem, 1, MPI_INT, pairRank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
//    } else {
//      MPI_Recv(&pairElem, 1, MPI_INT, pairRank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
//      MPI_Send(&procElem, 1, MPI_INT, pairRank, 0, MPI_COMM_WORLD);
//    }
    // Solution using MPI_SendRecv
    MPI_Sendrecv(&procElem, 1, MPI_INT, pairRank, 0, &pairElem, 1, MPI_INT, pairRank, 0, MPI_COMM_WORLD,
        MPI_STATUS_IGNORE);
    procElem = (procRank < pairRank) ? std::min(pairElem, procElem) : std::max(pairElem, procElem);
    pairMask = pairMask >> 1;
  }

  // Finally, gather the results back in the arr array of the master process
  MPI_Gather(&procElem, 1, MPI_INT, arr, 1, MPI_INT, 0, MPI_COMM_WORLD);
  // Do the same using the new gather implementation
  //MPI_GatherSingleInt(&procElem, arr, 0, MPI_COMM_WORLD);
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
    std::cout << "->The array before bitonic sort:\n";
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
      std::cout << "ERROR: Unknown array sort type " << argv[2] << ".\n";
    }
    return 1;
  }

  // Print the array and verify if it is sorted
  if (procRank == 0) {
    std::cout << "->The array after bitonic sort:\n";
    printArray(&arr[0], N);
    if (checkArrSorted(&arr[0], N)) {
      std::cout << "->The array is sorted correctly!\n";
    } else {
      std::cout << "->The array does not seem to be sorted!\n";
    }
  }

  MPI_Finalize();
  return 0;
}
