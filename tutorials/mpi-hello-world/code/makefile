EXECS=mpi_hello_world mpi_test mpi_test_weird
MPICC?=mpicc

all: ${EXECS}

mpi_hello_world: mpi_hello_world.c
	${MPICC} -o mpi_hello_world mpi_hello_world.c

mpi_test: mpi_test.c
	${MPICC} -o mpi_test mpi_test.c

mpi_test_weird: mpi_test_weird.c
	${MPICC} -g -o mpi_test_weird mpi_test_weird.c

clean:
	rm -f ${EXECS}
