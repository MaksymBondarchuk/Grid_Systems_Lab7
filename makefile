EXECS=tbb_integrate_right_rectangle

all: main

main: tbb_integrate_right_rectangle.cpp
	g++ -o ${EXECS} ${EXECS}.cpp -lm -ltbb

run:
	./${EXECS}

clean:
	rm ${EXECS}