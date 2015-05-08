#ifndef _PARSER_H_
#define _PARSER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <math.h>

#define bool int
#define true 1
#define false 0

#define PUNCT '.': \
		case '(': case ')': \
		case '[': case ']': \
		case '{': case '}'
#define SPACE ' ': \
		case ',': case '\n': \
		case '\r': case '\t'

enum Type {
	SYNTAX,
//	SYNTAX_OPEN_PAREN,
//	SYNTAX_CLOSE_PAREN,
//	SYNTAX_OPEN_SQUARE,
//	SYNTAX_CLOSE_SQUARE,
//	SYNTAX_OPEN_CURLY,
//	SYNTAX_CLOSE_CURLY,
	SYMBOL,
	CHAR,
	STRING,
	NUMBER,
	FLOAT
};

typedef struct Token {
	const char *token;
	int toklen;
	bool managed;
	enum Type type;
	struct Token *next;
	struct Token *prev;
	struct Arena *arena;
} Token;

typedef struct ArenaOptions {
	const size_t BUFFSIZE;
	const size_t MAXALLOCS;
	const double EXPONENT;
	int (*inc_factor)(const struct Arena *a);
} ArenaOptions;

typedef struct Arena {
	struct Token *first;
	struct Token *tptr;
	int multiplier;
	size_t numAllocs;
	struct Token **allocs;
	size_t numTokens;
	size_t totalNumTokens;
	const struct ArenaOptions *opts;
} Arena;

typedef struct Expression {
	const Token *function;
	int arity;
	struct Expression *child;
	struct Expression *sibling;
} Expression;


int
abs(int n);

const char *
tokens_to_string(const Token *token);

const char *
tokens_to_string2(const Token *token, const char sep);

void
print_tokens(const Token *tokens);

void *
die_parser_error(const char *msg, Arena *arena);

Token *
prior_expression(Token *token);

void
unlink_token(Token *token);

Token *
new_token();

void
free_tokens(Arena *arena);

Token *
inc_token(Token *token);

Token *
token_from_string(const char *str, Arena *arena);

Token *
token_from_char(char c, Arena *arena);

int
insert_tokens_after(Token *token, Token *after);

bool
is_numeric(char c);

bool
is_decimal_point(const char *dot, bool check_behind);

/**
 ** Replace '.' notation with functional equivalent
 **
 ***/
Arena *
normalize(Arena *arena);

ArenaOptions *
new_arena_options(
	size_t buffsize,
	size_t maxallocs,
	double exponent,
	int (*inc_factor)(const Arena *)
);

Arena *
tokenize2(const char *query, struct ArenaOptions *opts);

Arena *
tokenize3(const char *query, struct ArenaOptions *opts, bool verbose);

Arena *
tokenize(const char *query);

const Expression *
analyze(const Arena *arena);


#endif // _PARSER_H_

