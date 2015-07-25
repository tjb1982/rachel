#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include "parser.h"


struct NormalizeTest {
	const char *source;
	const char *expectation;
};

struct TypeTest {
	const char *source;
	enum Type type;
	int pos;
};

struct NormalizeTest normalize_tests[] = {
	{ "x", "x" },
	{ "\"x\"", "\"x\"" },
	{ "\"x.y.z().()\"", "\"x.y.z().()\"" },
	{ "\"x.y.z().()\"", "\"x.y.z().()\"" },
	{ "x.y", "y,(,x,)" },
	{ "().x", "x,(,(,),)" },
	{ "x.()", "(,),(,x,)" },
	{ "x.y.()", "(,),(,y,(,x,),)" },
	{ "().x.y", "y,(,x,(,(,),),)" },
	{ "().()", "(,),(,(,),)" },
	{ "x.y.z", "z,(,y,(,x,),)" },
	{ "z(x().y)", "z,(,y,(,x,(,),),)" },
	{ "z(x(\"foo\").y)", "z,(,y,(,x,(,\"foo\",),),)" },
	{ "z(x(\"f;oo\").y)", "z,(,y,(,x,(,\"f;oo\",),),)" },
	{ "z;(x(\"f;oo\").y)", "z" },
	{ "1.1 1.2", "1.1,1.2" },
	{ ".1 1.2", ".1,1.2" },
	{
		"buildings(@.appreciationRate.$gte(3.)).$map(@.appreciationRate).$apply($average)",
		"$apply,(,$map,(,buildings,(,$gte,(,appreciationRate,(,@,),3.,),),appreciationRate,(,@,),),$average,)"
	},
	{ "1.$gte(1.01)", "$gte,(,1,1.01,)" },
	{ "222.2.$gte(223)", "$gte,(,222.2,223,)" },
	{
		"buildings(@.appreciationRate.$gte(.01)).$map(@.appreciationRate).$apply($average)",
		"$apply,(,$map,(,buildings,(,$gte,(,appreciationRate,(,@,),.01,),),appreciationRate,(,@,),),$average,)"
	},
	{
		"test.buildings(@.marketValue.$gte(200000))",
		"buildings,(,test,$gte,(,marketValue,(,@,),200000,),)"
	},
	{
		"test.people.$map(@.children.$join(test.people))",
		"$map,(,people,(,test,),$join,(,children,(,@,),people,(,test,),),)"
	},
	{
		"test.people.$map(test.people.$join(@.children))",
		"$map,(,people,(,test,),$join,(,people,(,test,),children,(,@,),),)"
	},
	{ "+222.222", "+222.222" },
	{ "-222", "-222" },
	{ NULL, NULL }
};

struct TypeTest type_tests[] = {
	{ "\"foo\"", STRING, 0 },
	{ "foo(1)", NUMBER, 2 },
	{ "foo(1.1) 1.2", FLOAT, 4 },
	{ "-1", NUMBER, 0 },
	{ "+1", NUMBER, 0 },
	{ "-1.0", FLOAT, 0 },
	{ "-.1", FLOAT, 0 },
	{ "-.0", FLOAT, 0 },
	{ "-.0j", FLOAT, 0 },
	{ "0j", NUMBER, 0 },
	{ "(0j)", NUMBER, 1 },
	{ "(\"0j\")", STRING, 1 },
	{ "(j0)", SYMBOL, 1 },
	{ "()", SYNTAX, 1 },
	{ "buildings.address(\"lalala\")", SYNTAX, 1 },
	{ "buildings.address(\"lalala\")", STRING, 4 },
	{ "buildings.address(\"lalala\" +.0)", FLOAT, 5 },
	{ "buildings.address(\"lalala\" +0)", NUMBER, 5 },
	{ "buildings.address(\"lalala\") +0", NUMBER, 6 },
	{ "(", SYNTAX, 0 },
	{ "2.22.$gt(foo)", FLOAT, 0 },
	{ "2.22.$gt('f')", CHAR, 4 },
	{ "foo(\"lala\" .2)", FLOAT, 3 },
	{ NULL, -1, -1 }
};

int
test_normalize(struct NormalizeTest test, bool verbose, ArenaOptions *opts)
{
	Arena *arena = normalize(tokenize2(test.source, opts));
	int ret = 0;
	const char *result = tokens_to_string2(arena->first, ',');

	if (strcmp(result, test.expectation)) {
		fprintf(stderr,
			"Fail: %s\n, %s\n, %s\n\n",
			test.source,
			test.expectation,
			result
		);
		ret = 1;
	}
	else if (verbose) printf("PASS: %s\n, %s\n, %s\n\n", test.source, test.expectation, result);

	if (verbose) print_tokens(arena->first);

	free_tokens(arena);
	free((void *)result);
	return ret;
}

int test_parse_type(struct TypeTest test, bool verbose, ArenaOptions *opts)
{
	Arena *arena = tokenize2(test.source, opts);
	int ret = 0, pos = test.pos;
	Token *token = arena->first;
	while (token && pos--)
		token = token->next;

	if (!token) {
		fprintf(stderr,
			"Fail: Token at position %i not found.\n\n",
			test.pos
		);
		ret++;
	}
	else if (token->type != test.type) {
		fprintf(stderr,
			"--> %s\nFail: Type %s does not match token->type (%s) for token: ",
			test.source, type_to_string(test.type), type_to_string(token->type)
		);
		write(1, token->token, token->toklen); puts("\n");
		const char *token_string = tokens_to_string2(arena->first, ',');
		printf("%s\n", token_string);
		free((void *)token_string);
		ret++;
	}

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
	ArenaOptions *opts
) {
	int j = times, failed = 0;
	while (j--) {
		int i = 0;
		while (normalize_tests[i].source) {
			failed += test_normalize(
				normalize_tests[i],
				verbose,
				opts
			);
			i++;
		}
		i = 0;
		while (type_tests[i].source != NULL) {
			failed += test_parse_type(
				type_tests[i],
				verbose,
				opts
			);
			i++;
		}
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
	ArenaOptions *opts = new_arena_options(
		buffsize, maxallocs, exponent, NULL
	);
	int ret = test_all(iterations, verbose, opts);
	if (ret) printf("%i failed\n", ret);
	free((void *)opts);
	return ret;
}

