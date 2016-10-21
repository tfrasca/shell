CC=gcc
CFLAGS=-I -Wall -g
lib= -lpthread

%.o: %.c 
	$(CC) -c -o $@ $< $(CFLAGS)

shell:  main.o
	$(CC) -o myshell main.o  $(lib)
	export shell=${PWD}/myshell

mycat: mycat.o
	$(CC) -o mycat mycat.o $(lib)

clean:
	rm -f *.o core* *~ shell myshell
