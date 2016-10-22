CC=gcc
CFLAGS=-I -Wall -g
lib= -lpthread

%.o: %.c 
	$(CC) -c -o $@ $< $(CFLAGS)

shell:  main.o
	$(CC) -o myshell main.o  $(lib)

quittest: quittest.o
	$(CC) -o quittest quittest.o $(lib)

mycat: mycat.o
	$(CC) -o mycat mycat.o $(lib)

clean:
	rm -f *.o core* *~ shell myshell quittest mycat userManual
