CC=gcc
CC_FLAGS=-O3 -fopenmp


basic_scatter: basic_scatter.c
	$(CC) $(CC_FLAGS) -o basic_scatter basic_scatter.c timer.c

clean:
	rm -f *.o
	rm -f basic_scatter
