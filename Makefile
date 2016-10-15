CC=gcc
CFLAGS=-I -Wall -g
lib= -lpthread

%.o: %.c 
	$(CC) -c -o $@ $< $(CFLAGS)

shell:  main.o
	$(CC) -o myshell main.o  $(lib)
	echo 'alias shell=${PWD}/myshell'>>~/.bashrc

clean:
	rm -f *.o core* *~ shell
