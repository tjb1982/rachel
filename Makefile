all: debug
	clang main.c -o dlq -O3 DLQParser.o
debug: DLQParser.o 
	clang main.c -o dlq.debug -g DLQParser.o

DLQParser.o:
	clang -c -o DLQParser.o DLQParser.c

clean:
	rm -f *.o dlq
