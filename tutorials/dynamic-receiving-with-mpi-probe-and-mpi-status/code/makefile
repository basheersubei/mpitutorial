EXECS=check_status probe probe2
MPICC?=mpicc

all: ${EXECS}

check_status: check_status.c
	${MPICC} -o check_status check_status.c

probe: probe.c
	${MPICC} -o probe probe.c

probe2: probe2.c
	${MPICC} -o probe2 probe2.c

clean:
	rm -f ${EXECS}
