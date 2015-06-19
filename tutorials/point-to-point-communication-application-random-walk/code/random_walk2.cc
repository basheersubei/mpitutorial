// Author: Wes Kendall
// Copyright 2011 www.mpitutorial.com
// This code is provided freely with the tutorials on mpitutorial.com. Feel
// free to modify it for your own use. Any distribution of the code must
// either provide a link to www.mpitutorial.com or keep this header in tact.
//
// Example application of random walking using MPI_Send, MPI_Recv, and
// MPI_Probe.
//
#include <iostream>
#include <vector>
#include <cstdlib>
#include <time.h>
#include <mpi.h>
#include <unistd.h>

using namespace std;

typedef struct {
  int location;
  int num_steps_left_in_walk;
} Walker;


void calculate_subdomain(int domain_size, int world_size, int rank, int* subdomain_size, int* subdomain_start) {

  // the size of each subdomain (for each process) is the total size / number of processes
  *subdomain_size = domain_size / world_size;
  // where this processes's subdomain starts is the size of subdomains * number of this process
  *subdomain_start = *subdomain_size * rank;

  // in case there's a remainder of subdomain region, assign it all to the last process
  if (rank == world_size - 1)
    *subdomain_size += domain_size % *subdomain_size;
}

void setup_walkers(vector<Walker>* incoming_walkers, int subdomain_size, int subdomain_start, int number_of_walkers_in_proc, int max_walk_size) {
  for (int i = 0; i < number_of_walkers_in_proc; i++) {
    Walker walker;
    walker.location = (rand() / (float) RAND_MAX) * (subdomain_size - 1) + subdomain_start;
    walker.num_steps_left_in_walk = (rand() / (float) RAND_MAX) * max_walk_size;
    incoming_walkers->push_back(walker);
  }
}

void walk(Walker* walker, int subdomain_size, int subdomain_start, int domain_size, vector<Walker>* outgoing_walkers) {
  while(walker->num_steps_left_in_walk > 0) {
    
    walker->location++;
    walker->num_steps_left_in_walk--;
    
    // handle the case when this walker jumps off to another subdomain
    if (walker->location >= subdomain_size + subdomain_start) {
      // handle case when walker walks off the end (move it back to beginning)
      if (walker->location >= domain_size) {
        walker->location = 0;
      }
      outgoing_walkers->push_back(*walker);
      break;
    }

  }
}

void send_outgoing(vector<Walker>* outgoing_walkers, int world_size, int world_rank) {
  // send it to the next process (unless we roll over)
  int rank_to_send_to = (world_rank + 1) % world_size;

  MPI_Send((void *)outgoing_walkers->data(), outgoing_walkers->size() * sizeof(Walker), MPI_BYTE, rank_to_send_to, 0, MPI_COMM_WORLD);
  outgoing_walkers->clear();
}

void get_incoming(vector<Walker>* incoming_walkers, int world_size, int world_rank) {


  // always assume the process before us sent the incoming
  int rank_to_receive_from = (world_rank == 0) ? world_size -1 : world_rank - 1;
  MPI_Status st;
  int incoming_size;
  // probe and figure out how many incoming walkers we're getting
  MPI_Probe(rank_to_receive_from, 0, MPI_COMM_WORLD, &st);
  MPI_Get_count(&st, MPI_BYTE, &incoming_size);
  // resize the vector before filling it in
  incoming_walkers->resize(incoming_size / sizeof(Walker));
  MPI_Recv((void *)incoming_walkers->data(), incoming_size, MPI_BYTE, rank_to_receive_from, 0, MPI_COMM_WORLD, &st);

}

int main(int argc, char** argv) {
  int domain_size;
  int max_walk_size;
  int num_walkers_per_proc;

  if (argc < 4) {
    cerr << "Usage: random_walk domain_size max_walk_size "
         << "num_walkers_per_proc" << endl;
    exit(1);
  }
  domain_size = atoi(argv[1]);
  max_walk_size = atoi(argv[2]);
  num_walkers_per_proc = atoi(argv[3]);

  MPI_Init(NULL, NULL);
  int world_size;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  int world_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

  srand(time(NULL) * world_rank);
  int subdomain_start, subdomain_size;
  vector<Walker> incoming_walkers, outgoing_walkers;

  // find out subdomain_size and subdomain_start for the walkers that this process will
  // handle given the total world_size and the current process rank
  calculate_subdomain(domain_size, world_size, world_rank, &subdomain_size, &subdomain_start);

  cout << "process " << world_rank << " sub size: " << subdomain_size << " and sub start: " << subdomain_start << endl;
  // set up the initial walkers for this process (randomly placed within bounds)
  setup_walkers(&incoming_walkers, subdomain_size, subdomain_start, num_walkers_per_proc, max_walk_size);


  // now go over every walker and walk it one step
  for (int i = 0; i < incoming_walkers.size();  i++)
    walk(&incoming_walkers[i], subdomain_size, subdomain_start, domain_size, &outgoing_walkers);

  // cout << "Process " << world_rank << " debug, with walkers:" << endl;
  // for (int i = 0; i < incoming_walkers.size(); i++)
  //   cout << "walker" << i << " location:" << incoming_walkers[i].location << endl;

  // then check for any out of bounds walkers, and send them to the corresponding process
  send_outgoing(&outgoing_walkers, world_size, world_rank);

  // now check for any incoming walkers being sent from other processes
  get_incoming(&incoming_walkers, world_size, world_rank);
  // TODO once we receive incoming walkers, repeat above until enough




  cout << "Process " << world_rank << " done, with walkers:" << endl;
  for (int i = 0; i < incoming_walkers.size(); i++)
    cout << "walker" << i << " location:" << incoming_walkers[i].location << endl;
  MPI_Finalize();
  return 0;
}
