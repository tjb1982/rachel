#ifndef _DLQPARSER_H_
#define _DLQPARSER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define BUFFSIZE 4096

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
	SYMBOL,
	STRING_LITERAL,
	DECIMAL_LITERAL
};

typedef struct Token {
	const char *token;
	int toklen;
	bool managed;
	enum Type type;
	struct Token *next;
	struct Token *prev;
} Token;

typedef struct Expression {
	const Token *function;
	int arity;
	const struct Expression *child;
	const struct Expression *sibling;
} Expression;

static Token *tptr = NULL;
static int multiplier = 1;
static size_t numAllocs = 0;
static Token *allocs[100];
static size_t numTokens = 0;

void *
die_parser_error(const char *msg, Token *token);

Token *
prior_expression(Token *token);

void
unlink_token(Token *token);

Token *
new_token();

void
free_tokens (Token *tokens);

Token *
inc_token(Token *token);

Token *
token_from_string(const char *str);

Token *
token_from_char(char c);

int
insert_tokens_after(Token *token, Token *after);

bool
is_numeric(char c);

bool
is_float_dot(const char *dot, bool check_behind);

/**
 *	* Replace '.' notation with functional equivalent
 *	 *
 *		**/
Token *
normalize(Token *tokens);

Token *
tokenize(const char *query);



#endif


