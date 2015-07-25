#include "parser.h"

const char *types[] = {
	"syntax",
	"symbol",
	"char",
	"string",
	"number",
	"float"
};

const char *
type_to_string(enum Type type)
{
	return types[type];
}

int
abs(int n)
{
	const int ret[2] = { n, -n };
	return ret[n < 0];
}

const char *
tokens_to_string(const Token *token)
{
	return tokens_to_string2(token, '\0');
}

const char *
tokens_to_string2(const Token *token, const char sep)
{
	int size = 1, sep_not_null = (sep != '\0');
	const Token *rewind = token;
	const char *start;
	char *buf;

	while (token) {
		size += token->toklen + sep_not_null;

		if (
			token->type >= SYMBOL &&
			token->prev &&
			token->prev->type >= SYMBOL
		) size++;

		token = token->next;
	}

	start = buf = malloc(sizeof(char) * size);
	
	token = rewind;
	while (token) {
		if (
			sep == '\0' &&
			token->type >= SYMBOL &&
			token->prev &&
			token->prev->type >= SYMBOL
		) *buf++ = ' ';

		memcpy(buf, token->token, token->toklen);
		buf += token->toklen;
		if (sep_not_null) *buf++ = sep;

		token = token->next;
	}
	buf[0] = '\0';
	if (sep_not_null) *--buf = '\0';
	return start;
}

void
print_tokens(const Token *token)
{
	const Arena *arena = token->arena;
	while (token) {
		write(1, token->token, token->toklen);
		printf(" ");
		fflush(stdout);
		token = token->next;
	}
	puts("\n");

	printf("allocs(%li) tokens(%li)\n", arena->numAllocs, arena->totalNumTokens);
}

void *
die_parser_error(const char *msg, Arena *arena)
{
	const ArenaOptions *opts = arena->opts;
	free_tokens(arena);
	free((void *)opts);
	fprintf(stderr, "Parser error: %s\n", msg);
	exit(1);
	return NULL;
}

const char *
forward_to_matching_char(const char *query, int *toklen, char c)
{
	int tl = *toklen;
	query++; tl++;
	while (*query != c) {
		if (*query == '\0') {
			return NULL;
		}
		tl++; query++;
	}
	*toklen = tl;
	return query;
}

Token *
prior_expression(Token *token)
{
	int och = 0;
	Token *ret = token;

	if (!token->prev) return NULL;

	switch (*token->prev->token) {
	case '}':
		while (token->prev && *token->prev->token != '{')
			token = token->prev;
		return prior_expression(token->prev);
	case ')':
		och = 1;
		token = token->prev->prev;
		while (token && (och || *token->token == ')')) {
			if (*token->token == ')')
				och++;
			else if (*token->token == '(')
				och--;
			ret = token->prev ? (token = token->prev) : token;
		}
		break;
	default:
		ret = (token = token->prev) ? token : ret;
	}
	return ret;
}

Token *
end_of_expression(Token *token)
{
	Token *ret = token;
	int och = 0;
	if (!token->next) return token;

	switch (*token->token) {
	case '(':
		och = 1;
		token = token->next;
		while (token && och) {
			if (*token->token == '(')
				och++;
			else if (*token->token == ')')
				och--;
			ret = token;
			token = token->next;
		}
		break;
	default:
		if (*token->next->token == '(')
			ret = end_of_expression(token->next);
		else ret = token;
	}
	return ret;
}

void
unlink_token(Token *token)
{
	token->next->prev = 
	token->prev->next = NULL;
	token->next = token->prev = NULL;
}

int
inc_factor(const Arena *arena)
{
	return ceil(
		pow((double)arena->multiplier, (double)arena->opts->EXPONENT)
	);
}

Token *
new_token(Arena *arena)
{
	if (arena->numTokens == arena->opts->BUFFSIZE * arena->opts->inc_factor(arena)) {
		if (arena->numAllocs == arena->opts->MAXALLOCS) die_parser_error(
			"Maximum allocations reached. Aborting.",
			arena
		);
		arena->multiplier++;
		printf("alloc %i: %li tokens\n", arena->multiplier, arena->totalNumTokens);
		fflush(stdout);
		arena->tptr = arena->allocs[arena->numAllocs++] = malloc(
			sizeof(Token) *
			arena->opts->BUFFSIZE *
			arena->opts->inc_factor(arena)
		);
		arena->numTokens = 0;
	}
	arena->totalNumTokens++;
	arena->tptr->token = NULL;
	arena->tptr->toklen = 0;
	arena->tptr->managed = false;
	/* set the type to the first element of `enum Type` */
	arena->tptr->type = 0;
	arena->tptr->arena = arena;
	arena->tptr->next = arena->tptr->prev = NULL;

	arena->numTokens++;

	return arena->tptr++;
}

void
free_tokens (Arena *arena)
{
	Token *tokens = arena->first;
	while (tokens) {
		if (tokens->managed == true)
			free((void *)tokens->token);
		tokens = tokens->next;
	}
	while (arena->numAllocs) {
		free(arena->allocs[--arena->numAllocs]);
	}
	free(arena->allocs);
	free(arena);
}

Token *
inc_token(Token *token)
{
	Token *prev = token;
	token->next = new_token(token->arena);
	token = token->next;
	token->prev = prev;

	return token;
}

Token *
token_from_string(const char *str, Arena *arena)
{
	Token *token = new_token(arena);
	int toklen = (int) strlen(str);
	char *t = malloc(sizeof(char) * toklen);
	memcpy(t, str, toklen);
	token->token = t;
	token->toklen = toklen;
	token->managed = true;
	return token;
}

Token *
token_from_char(char c, Arena *arena)
{
	Token *token = new_token(arena);
	char *t = malloc(sizeof(char));
	*t = c;
	token->token = t;
	token->toklen = 1;
	token->managed = true;
	return token;
}

int
insert_tokens_after(Token *token, Token *after)
{
	Token *next, *end;
	int count = 1;

	next = token->next;
	end = after;

	while (end->next) {
		end = end->next;
		count++;
	}

	token->next = after;
	after->prev = token;
	end->next = next;
	if (next) next->prev = end;
	return count;
}

bool
is_numeric(char c)
{
	return c >= 0x30 && c <= 0x39;
}

bool
is_decimal_point(const char *dot, bool check_behind)
{
	return 
	*dot == '.' && (
		is_numeric(*(dot + 1)) || (
			(
				*(dot + 1) == ')' ||
				*(dot + 1) == '('
			) &&
			check_behind && is_numeric(*(dot - 1))
		)
	);
}

Token *
splice_token2(Token *remove, Token *prev)
{
	Token *next = remove->next;
	unlink_token(remove);
	next->prev = prev;
	if (next->prev) next->prev->next = next;
	return next;
}

Token *
splice_token(Token *remove)
{
	return splice_token2(remove, remove->prev);
}

Arena *
normalize(Arena *arena)
{
	Token *after, *expression, *next = arena->first;
	int ins = 0;

	while (next) {
		if (*next->token == '.' && next->toklen == 1) {
			if (
				!next->prev ||
				!next->next ||
				*next->next->token == '.' ||
				*next->next->token == ')' ||
				*next->prev->token == '(' ||
				(*next->next->token == '(' && !next->next->next)
			) {
				/* 
				 * can't start query with '.';
				 * can't have hanging '.' at the end;
				 * can't have two '.'s in a row;
				 * can't have a '(' or ')' as a symbol
				 * */
				die_parser_error(
					"illegal use of '.' token.", arena
				);
			}
			expression = prior_expression(next);
			next = splice_token2(next, expression->prev);
			if (expression == arena->first)
				arena->first = next; // reset the starting position

			after = end_of_expression(next);
			if (after->next && *after->next->token == '(')
				insert_tokens_after(after->next, expression);
			else if (next->next && *next->next->token == '(')
				insert_tokens_after(next->next, expression);
			else {
				insert_tokens_after(
					after, token_from_char('(', arena)
				);
				ins = insert_tokens_after(
					after->next, expression
				);
				while (--ins) expression = expression->next;
				insert_tokens_after(
					expression, token_from_char(')', arena)
				);
			}
		}
		else next = next->next;
	}
	return arena;
}

ArenaOptions *
new_arena_options(
	size_t buffsize,
	size_t maxallocs,
	double exponent,
	int (*custom_inc_factor)(const struct Arena *)
) {
	ArenaOptions opts = {
		.BUFFSIZE = buffsize,
		.MAXALLOCS = maxallocs,
		.EXPONENT = exponent,
		.inc_factor = custom_inc_factor ? custom_inc_factor : inc_factor
	};
	ArenaOptions *dest = malloc(sizeof (ArenaOptions));
	memcpy(dest, &opts, sizeof *dest);
	return dest;
}

Arena *
new_arena(const ArenaOptions *opts)
{
	Arena *arena = malloc(sizeof(Arena));
	arena->multiplier = 1;
	arena->numAllocs = arena->numTokens = arena->totalNumTokens = 0;
	arena->opts = opts;
	arena->allocs = malloc(opts->MAXALLOCS * sizeof (size_t));

	return arena;
}

Arena *
tokenize(const char *query)
{
	return tokenize3(query, new_arena_options(256, 100, 2.71828, NULL), false);
}

Arena *
tokenize2(const char *query, ArenaOptions *opts)
{
	return tokenize3(query, opts, false);
}

Arena *
tokenize3(const char *query, ArenaOptions *opts, bool verbose)
{
	int toklen = 0;
	Token *prev, *token;
	const char *orig = query;

	Arena *arena = new_arena(opts);

	arena->allocs[arena->numAllocs++] =
	arena->tptr =
	arena->first = malloc(
		sizeof(Token) *
		arena->opts->BUFFSIZE *
		arena->opts->inc_factor(arena)
	);
	token = new_token(arena);
	token->token = NULL;

	/**
	 * tokenize null terminated string by creating Tokens
	 * whose `token->token` point to the start of each 
	 * token and provide the length in token->toklen.
	 * __These aren't null-terminated "C strings"__
	 * */
	while (*query != '\0') {
		/**
		 * if token->toklen is assigned, this is the signal
		 * to increment to the next token and reset the
		 * `toklen` counter.
		 * */
		if (token->toklen) {
			token = inc_token(token);
			toklen = 0;
		}
		if (!token->token)
			token->token = query;

		switch (*query) {
		case ';':
			/* comments */
			while(*(query++ +1) != '\n' && *query != '\0')
				;
			break;
		case PUNCT:
			/* don't tokenize decimal points */
			if (is_decimal_point(query, orig != query)) {
				token->type = FLOAT;
				query++; toklen++;
			}
			else {
				if (toklen) {
					/** 
					 * End the current token, but don't increment the query pointer so
					 * that when we come back around we can account for the syntactic
					 * token in the `else` statement.
					 * */
					token->toklen = toklen;
				}
				else {
					token->toklen = 1;
					token->type = SYNTAX;
					query++;
				}
			}
			break;
		case SPACE:
			/* End the current token and increment the query pointer. */
			token->toklen = toklen;
			query++;
			break;
		default:

			if (token->type == UNDEFINED) {
				if (is_numeric(*query))
					token->type = NUMBER;
				else
					token->type = SYMBOL;
			}

			switch (*query) {
			case '-':
			case '+':
				if (is_numeric(*(query + 1)) || *(query + 1) == '.') {
					token->type = NUMBER;

					const char *num = query;
					while (is_numeric(*++num))
						;
					if (*num == '.')
						token->type = FLOAT;
				}
				toklen++;
				break;
			case '"':
				token->type = STRING;
				query = forward_to_matching_char(query, &toklen, '"');
				if (!query)
					die_parser_error("Assertion error: Unmatched double-quote (\")", arena);
				token->toklen = toklen + 1;
				break;
			case '\'':
				token->type = CHAR;
				query = query + 2;
				toklen = toklen + 2;
				if (*query != '\'')
					die_parser_error("Assertion error: Invalid use of single-quote (')", arena);
				token->toklen = toklen + 1;
				break;
			default:
				toklen++;
			}

			query++;
		}
	}
	if (!token->toklen) token->toklen = toklen;
	if (!token->token && token == arena->first) {
		free_tokens(arena);
		return NULL;
	}

	return arena;
}

void
free_expressions(const Expression *e)
{
	if (e->child)
		free_expressions(e->child);
	if (e->sibling)
		free_expressions(e->sibling);
	free((void *)e);
}

Expression *new_expression(const Token *function)
{
	Expression *e = malloc(sizeof(Expression));
	e->function = function;
	e->arity = 0;
	e->child = NULL;
	e->sibling = NULL;
	return e;
}

const Expression *
analyze(const Arena *arena)
{
	Expression *e = new_expression(arena->first);
//	e->child = new_expression(tokens);
//	e->sibling = new_expression(tokens);
//	e->sibling->sibling = new_expression(tokens);
	free_expressions(e);
	return e;
}

