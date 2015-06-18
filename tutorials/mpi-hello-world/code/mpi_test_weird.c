#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define ARRAY_SIZE 5

// searches for number in array from start index to end index
// returns 1 if found, 0 otherwise
int search(int* arr, int start, int end, int number) {
  for (; start < end; start++) {
    if (arr[start] == number)
      return 1;
  }
  return 0;
}

int main(int argc, char** argv) {
  // Initialize the MPI environment. The two arguments to MPI Init are not
  // currently used by MPI implementations, but are there in case future
  // implementations might need the arguments.
  MPI_Init(NULL, NULL);

  // Get the number of processes
  int world_size;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  // Get the rank of the process
  int world_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

  // Get the name of the processor
  char processor_name[MPI_MAX_PROCESSOR_NAME];
  int name_len;
  MPI_Get_processor_name(processor_name, &name_len);

  int found = 0;
  int find_me = 777;

  int A[ARRAY_SIZE];
  int i = 0;
  for (; i < ARRAY_SIZE; i++) {
    A[i] = rand() % 1000;
    printf("A[%d]: %d\n", i, A[i]);
  }

  int interval = ARRAY_SIZE / world_size;

  int res = search(A, world_rank * interval, (world_rank + 1) * interval, find_me);
  if (res == 1) {
    printf("found value %d in process %d\n", find_me, world_rank);
    // printf("found value %d in process %d!!! Killing rest of processes!\n", find_me, world_rank);
    // MPI_Finalize();
    found = 1;
    MPI_Request req;
    MPI_Ibcast(&found, 1, MPI_INT, world_rank, MPI_COMM_WORLD, &req);
  }


  // printf("rank is %d\n", world_rank);
  // if (world_rank == 0) {
  //   printf("rank %d is sleeping\n", world_rank);
  //   sleep(5);
  //   printf("rank %d woke up\n", world_rank);
  // }

  MPI_Barrier(MPI_COMM_WORLD);
  found = 0;
  MPI_Request r;
  MPI_Irecv(&found, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &r);
  if (found == 0) {
    printf("did not find value in any processes! Aborting!\n");
  }

  MPI_Finalize();
}
