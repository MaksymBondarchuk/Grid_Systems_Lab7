EXECS=main

all: main

main: main.cpp
	g++ -o ${EXECS} ${EXECS}.cpp -lm -ltbb

run:
	./${EXECS}

clean:
	rm ${EXECS}