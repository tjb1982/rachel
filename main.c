#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "parser.h"

int
main (int argc, char *argv []) {

	char c, *contents, *tempFile = ".dlqtemp", buf[1], *input_file_name, *strexec;
	size_t input_size = 0, buffsize = 255, maxallocs = 100;
	double exponent = 2.71828;
	FILE *input = NULL, *temp = NULL;
	bool verbose = false;

	if (argc > 1) {
		if(!strcmp(argv[1],"-")) {
			input = stdin;
		}
		else {
			while (~(c = getopt(argc, argv, "vb:a:x:e:i:"))) {
				switch (c) {
				case 'v':
					verbose = true;
					break;
				case 'b':
					buffsize = (size_t)strtol(optarg, NULL, 10);
					break;
				case 'a':
					maxallocs = (size_t)strtol(optarg, NULL, 10);
					maxallocs = ~maxallocs ? maxallocs : 9999999;
					break;
				case 'x':
					exponent = (double)strtof(optarg, NULL);
					break;
				case 'e':
					input_size = strlen(optarg) + 1;
					strexec = malloc(
						(sizeof(char) * input_size) + 1
					);
					memcpy(strexec, optarg, input_size);
					break;
				case 'i':
					input = fopen(optarg,"r");
					if (NULL == input) {
						fprintf(
							stderr,
							"Unable to open '%s': %s\n",
							optarg,
							strerror(errno)
						);
						exit(EXIT_FAILURE);
					}
					break;
				case '?':
					if (optopt == 'i')
					  fprintf (stderr, "Option -%c requires an argument.\n", optopt);
					else if (isprint (optopt))
					  fprintf (stderr, "Unknown option `-%c'.\n", optopt);
					else
					  fprintf (stderr,
						   "Unknown option character `\\x%x'.\n",
						   optopt);
					return 1;
				}
			}
		}
	}
	else {
		input = stdin;
	}

	temp = fopen(tempFile, "w");
	if (!input_size) while (EOF != (buf[0] = fgetc(input))) {
		input_size += fwrite(buf, sizeof(char), 1, temp);
	}
	else {
		fwrite(strexec, sizeof(char), input_size, temp);
		free(strexec);
	}
	fclose(temp);
	if (input) fclose(input);

	temp = fopen(tempFile, "r");
	contents = malloc (
		input_size * sizeof(char) + 1
	);
	fread (
		contents,
		sizeof (char),
		input_size,
		temp
	);
	contents[sizeof(char) * input_size] = '\0';
	fclose (temp);
	remove(tempFile);

	ArenaOptions *opts = new_arena_options(
		buffsize, maxallocs, exponent, NULL 
	);

	/* create doubly linked list of tokens */
	Arena *arena = tokenize3(contents, opts, verbose);

	/* replace '.' notation with functional equivalent */
	normalize (arena);

	/* create abstract syntax tree from Tokens */
	const Expression *program = analyze (arena);

	if (verbose) print_tokens(arena->first);
	free_tokens (arena);
	free(opts);
	free (contents);
	return EXIT_SUCCESS;
}

