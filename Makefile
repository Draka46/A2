CC = gcc
CFLAGS = -Wall -std=c11 -g -ggdb

all: sim

../src.zip: Makefile *.h *.c test.sh tests/*.prime
	cd .. && zip src.zip src/*.h src/*.c src/Makefile src/test.sh src/tests/*.prime


sim: *.c *.h
	$(CC) $(CFLAGS) *.c -o sim

clean:
	rm -f *.o sim
