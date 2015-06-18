// Author: Wes Kendall
// Copyright 2011 www.mpitutorial.com
// This code is provided freely with the tutorials on mpitutorial.com. Feel
// free to modify it for your own use. Any distribution of the code must
// either provide a link to www.mpitutorial.com or keep this header in tact.
//
// Example of using MPI_Probe to dynamically allocated received messages
//
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char** argv) {
  MPI_Init(NULL, NULL);

  int world_size;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  // if (world_size != 2) {
  //   fprintf(stderr, "Must use two processes for this example\n");
  //   MPI_Abort(MPI_COMM_WORLD, 1);
  // }
  int world_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  const int MAX_NUMBERS = 100;
  int numbers[MAX_NUMBERS];
  int i;
  for (i = 0; i < MAX_NUMBERS; i++)
    numbers[i] = (rand() / (float)RAND_MAX) * MAX_NUMBERS;

  int number_amount;
  if (world_rank == 0) {

    srand(time(NULL));
    number_amount = (rand() / (float)RAND_MAX) * MAX_NUMBERS;

    // Broadcast the number of ints we will send
    MPI_Bcast(&number_amount, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // send the actual ints
    MPI_Bcast(numbers, number_amount, MPI_INT, 0, MPI_COMM_WORLD);
    printf("process %d sent %d numbers.\n", world_rank, number_amount);
  } else {
    MPI_Status stat;
    // get the number of ints we will receive
    MPI_Bcast(&number_amount, 1, MPI_INT, 0, MPI_COMM_WORLD);  
    int *received_numbers = (int *)malloc(number_amount * sizeof(int));

    // get the actual ints
    MPI_Bcast(received_numbers, number_amount, MPI_INT, 0, MPI_COMM_WORLD);

    printf("process %d received %d numbers: ", world_rank, number_amount);

    for (i = 0; i < number_amount; i++)
      printf("%d, ", received_numbers[i]);
    printf("\n");
    free(received_numbers);
  }
  MPI_Finalize();
}
