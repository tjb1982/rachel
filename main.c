#include "DLQParser.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

const char *normalize_tests[] = {
	"x", "x",
	"x.y", "y(x)",
	"().x", "x(())",
	"x.()", "()(x)",
	"x.y.()", "()(y(x))",
	"().x.y", "y(x(()))",
	"().()", "()(())",
	"x.y.z", "z(y(x))",
	"z(x().y)", "z(y(x()))",
	"z(x(\"foo\").y)", "z(y(x(\"foo\")))",
	"z(x(\"f;oo\").y)", "z(y(x(\"f;oo\")))",
	"z;(x(\"f;oo\").y)", "z",

	"test.buildings(@.marketValue.$gte(200000))",
	"buildings(test $gte(marketValue(@)200000))",

	"test.people.$map(@.children.$join(test.people))",
	"$map(people(test)$join(children(@)people(test)))",

	"test.people.$map(test.people.$join(@.children))",
	"$map(people(test)$join(people(test)children(@)))",

	NULL
};

int
test_normalize(const char *source, const char *expectation)
{
	Arena *arena = normalize(tokenize(source));
	int ret = 0;
	const char *result = tokens_to_string(arena->first);

	if (strcmp(result, expectation)) {
		fprintf(stderr,
			"Fail: %s, %s, %s\n\n",
			source,
			expectation,
			result
		);
		ret = 1;
	}
	else printf("PASS: %s, %s, %s\n", source, expectation, result);

	print_tokens(arena->first);

	free_tokens(arena);
	free((void *)result);
	return ret;
}

void
test_all()
{
	int i = 0, j = 10;
	while (j--) {
		while (normalize_tests[i] && normalize_tests[i+1]) {
			test_normalize(normalize_tests[i], normalize_tests[i+1]);
			i = i + 2;
		}
		i = 0;
	}
}

int
main (int argc, char *argv []) {
	/* copy contents of file into buffer "contents" */
	char c, *contents, *tempFile = ".dlqtemp", buf[1], *input_file_name;
	size_t input_size = 0;
	FILE *input = NULL, *temp = NULL;
	bool test = false;
	
	if (argc > 1) {
		if(!strcmp(argv[1],"-")) {
			input = stdin;
		}
		else {
			while (~(c = getopt(argc, argv, "ti:"))) {
				switch (c) {
				case 't':
					test = true;
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

	if (test == true) {
		test_all();
		return 0;
	}

	if (!input) return 1;

	temp = fopen(tempFile, "w");
	while (EOF != (buf[0] = fgetc(input))) {
		input_size += fwrite(buf, sizeof(char), 1, temp);
	}
	fclose(temp);
	fclose(input);

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

	/* create doubly linked list of tokens */
	Arena *arena = tokenize (contents);

	/* replace '.' notation with functional equivalent */
	normalize (arena);

	/* create abstract syntax tree from Tokens */
//	const Expression *program = analyze (arena);

	//print_tokens(arena->first);
	free_tokens (arena);
	free (contents);
	return 0;
}

