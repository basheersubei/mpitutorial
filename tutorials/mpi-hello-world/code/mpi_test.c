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
  MPI_Request req;
  int val = 777;


  int receive[4];
  receive[0] = 777; receive[1]= 777; receive[2] = 777; receive[3] = 777; 
  if(world_rank == 0) {
    MPI_Gather(&world_rank, 1, MPI_INT,
               receive, 4, MPI_INT,
               world_rank, MPI_COMM_WORLD);
    sleep(1);
  } else { 

    MPI_Gather(&world_rank, 1, MPI_INT,
               receive, 4, MPI_INT,
               0, MPI_COMM_WORLD);

  }
  // MPI_Ibcast(&world_rank, 1, MPI_INT, world_rank, MPI_COMM_WORLD, &req);
  // MPI_Bcast(&val, 1, MPI_INT, 0, MPI_COMM_WORLD);
  printf("process %d found receive %d %d %d %d\n", world_rank, receive[0], receive[1], receive[2], receive[3]);
  // MPI_Irecv(&found, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &r);
  MPI_Finalize();
}
