all: debug
	clang DLQParser.c -o dlq -O3
debug: 
	clang DLQParser.c -o dlq.debug -g
