CC = gcc
CFLAGS = -O3

bzip-table : bzip-table.o micro-bunzip.o

clean:
	rm -f *.o bzip-table
