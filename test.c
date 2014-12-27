#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include "DLQParser.h"

const char *normalize_tests[] = {
	"x", "x",
	"\"x\"", "\"x\"",
	"\"x.y.z().()\"", "\"x.y.z().()\"",
	"\"x.y.z().()\"", "\"x.y.z().()\"",
	"x.y", "y ( x )",
	"().x", "x ( ( ) )",
	"x.()", "( ) ( x )",
	"x.y.()", "( ) ( y ( x ) )",
	"().x.y", "y ( x ( ( ) ) )",
	"().()", "( ) ( ( ) )",
	"x.y.z", "z ( y ( x ) )",
	"z(x().y)", "z ( y ( x ( ) ) )",
	"z(x(\"foo\").y)", "z ( y ( x ( \"foo\" ) ) )",
	"z(x(\"f;oo\").y)", "z ( y ( x ( \"f;oo\" ) ) )",
	"z;(x(\"f;oo\").y)", "z",

	"1.1 1.2", "1.1  1.2",
	"buildings(@.appreciationRate.$gte(3.)).$map(@.appreciationRate).$apply($average)",
	"$apply ( $map ( buildings ( $gte ( appreciationRate ( @ ) 3. ) ) appreciationRate ( @ ) ) $average )",

	"1.$gte(1.01)", "$gte ( 1  1.01 )",
	"222.2.$gte(223)", "$gte ( 222.2  223 )",

	"buildings(@.appreciationRate.$gte(.01)).$map(@.appreciationRate).$apply($average)",
	"$apply ( $map ( buildings ( $gte ( appreciationRate ( @ ) .01 ) ) appreciationRate ( @ ) ) $average )",

	"test.buildings(@.marketValue.$gte(200000))",
	"buildings ( test  $gte ( marketValue ( @ ) 200000 ) )",

	"test.people.$map(@.children.$join(test.people))",
	"$map ( people ( test ) $join ( children ( @ ) people ( test ) ) )",

	"test.people.$map(test.people.$join(@.children))",
	"$map ( people ( test ) $join ( people ( test ) children ( @ ) ) )",

	NULL
};

int
test_normalize(const char *source, const char *expectation, bool verbose, const ArenaOptions *opts)
{
	Arena *arena = normalize(tokenize2(source, (ArenaOptions *)opts));
	int ret = 0;
	const char *result = tokens_to_string2(arena->first, ' ');

	if (strcmp(result, expectation)) {
		fprintf(stderr,
			"Fail: %s, %s, %s\n\n",
			source,
			expectation,
			result
		);
		ret = 1;
	}
	else if (verbose) printf("PASS: %s, %s, %s\n", source, expectation, result);

	if (verbose) print_tokens(arena->first);

	free_tokens(arena);
	free((void *)result);
	return ret;
}

int
test_analyze(Arena *arena, bool verbose)
{
	const Expression *e = analyze(arena);
	return 0;
}

int
test_all(
	int times,
	bool verbose,
	const ArenaOptions *opts
) {
	int i = 0, j = times, failed = 0;
	while (j--) {
		while (normalize_tests[i] && normalize_tests[i+1]) {
			failed += test_normalize(
				normalize_tests[i],
				normalize_tests[i+1],
				verbose,
				opts
			);
			i = i + 2;
		}
		i = 0;
	}
	return failed;
}

int custom_inc_factor(const Arena *a) {
	return ceil(pow(a->multiplier, 2.71828));
}

int
main (int argc, char *argv []) {

	char c;
	bool verbose = false;
	int iterations = 1;
	size_t maxallocs = 100, buffsize = 255;
	double exponent = 2.71828;

	if (argc > 1) {
		while (~(c = getopt(argc, argv, "vn:b:a:x:"))) {
			switch (c) {
			case 'n':
				iterations = abs(
					(int)strtol(optarg, NULL, 10)
				);
				break;
			case 'v':
				verbose = true;
				break;
			case 'b':
				buffsize = (size_t)strtol(optarg, NULL, 10);
				if (!buffsize) {
					fprintf(stderr, "-%c argument invalid. Aborting test.\n", c);
					exit(EXIT_FAILURE);
				}
				break;
			case 'a':
				maxallocs = (size_t)strtol(optarg, NULL, 10);
				if (!maxallocs) {
					fprintf(stderr, "-%c argument invalid. Aborting test.\n", c);
					exit(EXIT_FAILURE);
				}
				maxallocs = ~maxallocs? maxallocs : 9999999;
				break;
			case 'x':
				exponent = (double)strtof(optarg, NULL);
				break;
			case '?':
				if (optopt == 'i') fprintf (
					stderr,
					"Option -%c requires an argument.\n",
					optopt
				);
				else if (isprint (optopt)) fprintf (
					stderr,
					"Unknown option `-%c'.\n",
					optopt
				);
				else fprintf (
					stderr,
					"Unknown option character `\\x%x'.\n",
					optopt
				);
				return 1;
			}
		}
	}
	const ArenaOptions *opts = new_arena_options(
		buffsize, maxallocs, exponent, NULL
	);
	int ret = test_all(iterations, verbose, opts);
	free((void *)opts);
	return ret;
}

