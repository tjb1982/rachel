all: debug rachel racheltest

rachel:
	clang main.c -o $@ parser.o -lm

debug: parser.o 
	clang main.c -o rachel.debug -g parser.o -lm

parser.o: 
	clang -c -o parser.o parser.c

racheltest: parser.o
	clang test.c -o racheltest -g -O3 parser.o -lm

clean:
	rm -rf *.o rachel*
