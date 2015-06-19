#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// searches for number in array from start index to end index
// returns index if found, -1 otherwise
int search(int* arr, int start, int end, int number) {
  for (; start < end; start++) {
    if (arr[start] == number)
      return start;
  }
  return -1;
}

int main(int argc, char** argv) {
  

  if (argc < 2) {
    printf("Usage: mpi_test_weird array_size\n"); 
    exit(1);
  }

  // Initialize the MPI environment. The two arguments to MPI Init are not
  // currently used by MPI implementations, but are there in case future
  // implementations might need the arguments.
  MPI_Init(NULL, NULL);

  double start_time = MPI_Wtime();

  srand(time(NULL));
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
  int find_me = 772;
  int array_size = atoi(argv[1]);
  int *A = (int *)malloc(array_size * sizeof(int));
  int i;
  
  // create an array if you're root process, then broadcast it
  if (world_rank == 0) {
    for (i = 0; i < array_size; i++) {
      A[i] = rand() % 1000;
      printf("A[%d]: %d\n", i, A[i]);
    }
  } else {
    for (i = 0; i < array_size; i++) {
      A[i] = 0;
      // printf("A[%d]: %d\n", i, A[i]);
    }
  }
  MPI_Bcast(A, array_size, MPI_INT, 0, MPI_COMM_WORLD);
  // now the non-root processes have the same array A as the root process (randomly generated array)

  // printf("process %d got array:\n", world_rank);
  // for (i = 0; i < array_size; i++)
  //   printf("[%d]: %d\n", i, A[i]);


  int interval = array_size / world_size;
  interval = (interval <= 0) ? 1 : interval;
  int start = world_rank * interval;
  // make sure the end index doesn't go out of bounds, also throw the rest onto the last process
  int end = (start + interval);
  end = (world_rank == world_size - 1) ? end + array_size % (world_size * interval) : end;
  
  if (start >= array_size || end > array_size) {
    printf("too many processes to divide array! Aborting MPI!\n");
    MPI_Abort(MPI_COMM_WORLD, 1);
    exit(1);
  }

  printf("process %d will search from start %d till end %d\n", world_rank, start, end);

  
  int res = search(A, start, end, find_me);
  if (res != -1) {
    printf("found value %d in process %d at index %d\n", find_me, world_rank, res);
    // printf("found value %d in process %d!!! Killing rest of processes!\n", find_me, world_rank);
    // MPI_Finalize();
    found = 1;
  }

  int* send_array = (int *)malloc(world_size * sizeof(int));
  for (i = 0; i < world_size; i++) {
    send_array[i] = 0;
    if (i == world_rank)
      send_array[i] = found;
  }
  // now send_array contains the found result in the index of this process rank
  int* receive_array = (int *)malloc(world_size * sizeof(int));
  MPI_Gather(
    &send_array[world_rank], 1, MPI_INT,
    receive_array, 1, MPI_INT,
    0, MPI_COMM_WORLD);
  // in the case of root process (rank 0), receive_array will have found values

  // if root process, go through all the found values to see if any processes found it
  if (world_rank == 0) {
    found = 0;  // reuse this variable
    for (i = 0; i < world_size; i++) {
      if (receive_array[i] == 1) {
        printf("Process %d found value!", i);
        found = 1;
      }
    }
    // if we went through array and didn't find anything
    if (!found) {
      printf("None of the processes found value %d!\n", find_me);
    }    
  }

  free(send_array);
  if (world_rank == 0)
    free(receive_array);
  free(A);
  // printf("rank is %d\n", world_rank);
  // if (world_rank == 0) {
  //   printf("rank %d is sleeping\n", world_rank);
  //   sleep(5);
  //   printf("rank %d woke up\n", world_rank);
  // }

  // if (found == 0) {
  //   printf("did not find value in any processes! Aborting!\n");
  // }

  double end_time = MPI_Wtime();

  if (world_rank == 0)
    printf("the total time taken: %f seconds\n", end_time - start_time);
  MPI_Finalize();
  return 0;
}
