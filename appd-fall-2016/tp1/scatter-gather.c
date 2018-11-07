#include <stdio.h>
#include "mpi.h"

// New scatter and gather implementations signatures for part 3

int MPI_ScatterSingleInt(
  const int *sendbuf,
  int *recvbuf,
  int root,
  MPI_Comm comm)
{
  // IMPLEMENT NEW SCATTER HERE!

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
    MPI_Status status;
    MPI_Recv(recvbuf, 1, MPI_INT, root, 0, MPI_COMM_WORLD, &status); // Receive my element from the root.
  }
  
  return MPI_SUCCESS;
}

int MPI_GatherSingleInt(
  const int *sendbuf,
  int *recvbuf,
  int root,
  MPI_Comm comm)
{
  // IMPLEMENT NEW GATHER HERE!

  // Get the processor rank and the number of processes.
  int procRank; MPI_Comm_rank(comm, &procRank);
  int numProcs; MPI_Comm_size(comm, &numProcs);

  if (procRank == root) { // Scatter sendbuf to all processes.
    MPI_Status status;
    for (int i = 0; i < numProcs; i++) {
      if (i == procRank) {
        recvbuf[i] = sendbuf[0]; // Directly copy the element for the root process.
      } else {
        MPI_Recv(recvbuf + i, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status); // Receive each process's element.
      }
    }
  } else {
    MPI_Send(sendbuf, 1, MPI_INT, root, 0, MPI_COMM_WORLD); // Send my element to the root.
  }
  
  return MPI_SUCCESS;
}
