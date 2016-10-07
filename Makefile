CC=gcc
CFLAGS=-I -Wall -g
lib= -lpthread

%.o: %.c 
	$(CC) -c -o $@ $< $(CFLAGS)

shell:  main.o
	$(CC) -o shell main.o  $(lib)

clean:
	rm -f *.o core* *~ shell
