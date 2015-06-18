EXECS=mpi_hello_world mpi_test
MPICC?=mpicc

all: ${EXECS}

mpi_hello_world: mpi_hello_world.c
	${MPICC} -o mpi_hello_world mpi_hello_world.c

mpi_test: mpi_test.c
	${MPICC} -o mpi_test mpi_test.c

clean:
	rm -f ${EXECS}
