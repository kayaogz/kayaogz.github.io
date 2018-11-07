// int *arr;       // The array of elements to sort. It exists only in the master rank, and is NULL in other ranks.
// int N;          // The number of elements, and the size of arr. Equally, the number of available MPI ranks.
// int procElem;   // The element assigned to the current process in bitonic sort.
// int procRank;   // The rank of current process
// int numProcs;   // The number of processes. Same as N.

// New scatter and gather signatures for the question 3
int MPI_ScatterSingleInt(
  const int *sendbuf,
  int *recvbuf,
  int root,
  MPI_Comm comm);

int MPI_GatherSingleInt(
  const int *sendbuf,
  int *recvbuf,
  int root,
  MPI_Comm comm);

// IMPLEMENT PARALLEL BITONIC SORT HERE!

#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define MIN(a,b) ((a) < (b) ? (a) : (b))

// First scatter the elements of arr in the master process to others so that each process has its element in procElem.
MPI_Scatter(arr, 1, MPI_INT, &procElem, 1, MPI_INT, 0, MPI_COMM_WORLD);
// Using the new scatter implementation
//MPI_ScatterSingleInt(arr, &procElem, 0, MPI_COMM_WORLD);

// Then perform the bitonic separator for logN steps
int pairElem;
int pairMask = numProcs >> 1; // Flipping this bit should give my pair at each round
MPI_Status status;

while (pairMask > 0) {
  int pairRank = procRank ^ pairMask;
  // Solution using separate MPI_Send and MPI_Recv
  if (pairRank < procRank) { 
    MPI_Send(&procElem, 1, MPI_INT, pairRank, 0, MPI_COMM_WORLD);
    MPI_Recv(&pairElem, 1, MPI_INT, pairRank, 0, MPI_COMM_WORLD, &status);
  } else {
    MPI_Recv(&pairElem, 1, MPI_INT, pairRank, 0, MPI_COMM_WORLD, &status);
    MPI_Send(&procElem, 1, MPI_INT, pairRank, 0, MPI_COMM_WORLD);
  }
  // Solution using MPI_SendRecv
  // MPI_Sendrecv(&procElem, 1, MPI_INT, pairRank, 0, &pairElem, 1, MPI_INT, pairRank, 0, MPI_COMM_WORLD, &status);
  procElem = (pairRank > procRank) ? MIN(pairElem, procElem) : MAX(pairElem, procElem);
  pairMask = pairMask >> 1;
}

// Finally, gather the results back in the arr array of the master process
MPI_Gather(&procElem, 1, MPI_INT, arr, 1, MPI_INT, 0, MPI_COMM_WORLD);
// Using the new gather implementation
//MPI_GatherSingleInt(&procElem, arr, 0, MPI_COMM_WORLD);
