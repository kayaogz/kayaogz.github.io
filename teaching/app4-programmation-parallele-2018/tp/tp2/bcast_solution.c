// Already defined variables in the original source code. Do not uncomment any of those, just use them directly!
// Process rank 0 should be the source of the broadcast
//
// int num_procs;
// int rank;
// char *bcast_implementation_name:   the bcast implementation name (argument #1)
// int chunk_size:                    the chunk size (optional argument #2)
// int NUM_BYTES:                     the number of bytes to broadcast
// char *buffer:                      the buffer to broadcast
//
// The method names should be:
// default_bcast
// naive_bcast
// ring_bcast
// pipelined_ring_bcast
// asynchronous_pipelined_ring_bcast
// asynchronous_pipelined_bintree_bcast
//
// GOOD LUCK (gonna need it)!

if (strcmp(bcast_implementation_name, "default_bcast") == 0) { // Just calling the library routine.
  MPI_Bcast(buffer, NUM_BYTES, MPI_CHAR, 0, MPI_COMM_WORLD);
} else if (strcmp(bcast_implementation_name, "naive_bcast") == 0) { // Send to all processes one-by-one from the root.
}
