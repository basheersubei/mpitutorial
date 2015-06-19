EXECS=random_walk random_walk2
MPICXX?=mpicxx

all: ${EXECS}

random_walk: random_walk.cc
	${MPICXX} -o random_walk random_walk.cc

random_walk2: random_walk2.cc
	${MPICXX} -o random_walk2 random_walk2.cc

clean:
	rm -f ${EXECS}
