EXECS=main
MPICC?=g++

all: ${EXECS}

main: main.cpp
	${MPICC} -o ${EXECS} ${EXECS}.cpp -lm -ltbb

run:
	./${EXECS}

clean:
	rm ${EXECS}