all: debug dlq dlqtest

dlq:
	clang main.c -o dlq -O3 DLQParser.o -lm

debug: DLQParser.o 
	clang main.c -o dlq.debug -g DLQParser.o -lm

DLQParser.o:
	clang -c -o DLQParser.o DLQParser.c 

dlqtest: DLQParser.o
	clang test.c -o dlqtest -g -O3 DLQParser.o -lm

clean:
	rm -f *.o dlq*
