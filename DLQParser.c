#include "DLQParser.h"

void *
die_parser_error(const char *msg, Token *token)
{
	free_tokens(token);
	fprintf(stderr, "Parser error: %s\n", msg);
	exit(1);
	return NULL;
}

//int
//assert_char_pair(const char *query, Token *token, int *toklen, char c1, char c2)
//{
//
//		och = 1;
//		token = token->prev->prev;
//		while (token && (och || *token->token == ')')) {
//			if (*token->token == ')')
//				och++;
//			else if (*token->token == '(')
//				och--;
//			ret = (token = token->prev) ? token : ret;
//		}
//		break;
//
//
//
//	int tl = *toklen;
//	if (*query == c) {
//		query++; tl++;
//		while (*query != c) {
//			if (*query == '\0') {
//				return 1;
//			}
//			tl++; query++;
//		}
//		token->toklen = ++tl;
//		*toklen = 0;
//	}
//	return 0;
//}

const char *
forward_to_matching_char(const char *query, Token *token, int *toklen, char c)
{
	int tl = *toklen;
	query++; tl++;
	while (*query != c) {
		if (*query == '\0') {
			return NULL;
		}
		tl++; query++;
	}
	token->toklen = ++tl;
	*toklen = 0;
	return query;
}

Token *
prior_expression(Token *token)
{
	int och = 0;
	Token *ret = token;

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
			ret = (token = token->prev) ? token : ret;
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

Token *
new_token()
{
	if (numTokens == BUFFSIZE * multiplier) {
		tptr = allocs[numAllocs++] = malloc(
			sizeof(Token) * BUFFSIZE * ++multiplier
		);
		numTokens = 0;
	}
	tptr->token = NULL;
	tptr->toklen = 0;
	tptr->managed = false;
	tptr->type = SYNTAX;
	tptr->next = tptr->prev = NULL;

	numTokens++;

	return tptr++;
}

void
free_tokens (Token *tokens)
{
	Token *prev;
	while (tokens) {
		if (tokens->managed == true)
			free((void *)tokens->token);
		tokens = tokens->next;
	}
	while (numAllocs) {
		free(allocs[--numAllocs]);
	}
	numTokens = 0;
}

Token *
inc_token(Token *token)
{
	Token *prev = token;
	token->next = new_token();
	token = token->next;
	token->prev = prev;

	return token;
}

Token *
token_from_string(const char *str)
{
	Token *token = new_token();
	int toklen = (int) strlen(str);
	char *t = malloc(sizeof(char) * toklen);
	memcpy(t, str, toklen);
	token->token = t;
	token->toklen = toklen;
	token->managed = true;
	return token;
}

Token *
token_from_char(char c)
{
	Token *token = new_token();
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
is_float_dot(const char *dot, bool check_behind)
{
	return *dot == '.' && is_numeric(
		*(dot + 1) || (check_behind && is_numeric(*(dot - 1)))
	);
}

Expression *new_expression(const Token *function)
{
	Expression *e = malloc(sizeof(Expression));
	e->function = NULL;
	e->arity = 0;
	e->child = NULL;
	e->sibling = NULL;
	return e;
}

const Expression *
analyze(const Token *tokens)
{
	Expression *e = new_expression(tokens);
	e->function = tokens;
	return e;
}

Token *
splice_token2(Token *remove, Token *prev)
{
	Token *next = remove->next;
	unlink_token(remove);
	next->prev = prev;
	return next->prev ? (next->prev->next = next) : next;
}

Token *
splice_token1(Token *remove)
{
	return splice_token2(remove, remove->prev);
}

Token *
normalize(Token *tokens)
{
	Token *after, *expression, *next = tokens;
	int ins = 0;

	while (next) {
		if (*next->token == '.' && next->toklen == 1) {
			if (
				next == tokens ||
				!next->next ||
				*next->next->token == '.' ||
				*next->next->token == ')' ||
				(*next->next->token == '(' && !next->next->next) ||
				(next->prev && *next->prev->token == '(')
			) {
				/* 
				 * can't start query with '.';
				 * can't have hanging '.' at the end;
				 * can't have two '.'s in a row;
				 * can't have a '(' or ')' as a symbol
				 * */
				die_parser_error(
					"illegal use of '.' token.", tokens
				);
			}
			expression = prior_expression(next);
			next = splice_token2(next, expression->prev);
			if (expression == tokens)
				tokens = next; // reset the starting position

			after = end_of_expression(next);
			if (after->next && *after->next->token == '(')
				insert_tokens_after(after->next, expression);
			else if (next->next && *next->next->token == '(')
				insert_tokens_after(next->next, expression);
			else {
				insert_tokens_after(
					after, token_from_char('(')
				);
				ins = insert_tokens_after(
					after->next, expression
				);
				while (--ins) expression = expression->next;
				insert_tokens_after(
					expression, token_from_char(')')
				);
			}
		}
		else next = next->next;
	}
	return tokens;
}

Token *
tokenize(const char *query)
{
	int toklen = 0;
	bool stop = false;
	Token *first, *prev, *token;
	const char *orig = query;

	allocs[numAllocs++] = tptr = malloc(
		sizeof(Token) * BUFFSIZE * multiplier
	);
	token = first = new_token();

	token->token = NULL;

	/* tokenize char[] by creating Tokens whose 
	 * token->token point to the start of each 
	 * token and provide the length in token->toklen.
	 * THESE ARE NOT NULL TERMINATED "C STRINGS" */
	while (*query != '\0') {
		stop = false;
		switch (*query) {
		case ';':
			/* comments */
			while(*(query++ +1) != '\n' && *query != '\0')
				;
			break;
		case PUNCT:
			/* don't tokenize floats */
			if (is_float_dot(query, orig != query)) {
				query++; toklen++;
				break;
			}
			if (!token->toklen) token->toklen = toklen;

			token = inc_token(token);
			token->token = query++;
			token->toklen = 1;
			token->type = SYNTAX;

			toklen = 0;
			break;
		case SPACE:
			if (!token->toklen)
				token->toklen = toklen;
			toklen = 0;
			while (!stop) {
				switch (*query) {
				case SPACE:
					query++;
					break;
				default:
					stop = true;
					break;
				}
			}
			break;
		default:
			if (!token->token)
				token->token = query;
			if (token->toklen) {
				token = inc_token(token);
				token->token = query;
			}

			token->type = SYMBOL;

			if (is_numeric(*token->token))
				token->type = DECIMAL_LITERAL;
			
			*query == '"' &&
			(token->type = STRING_LITERAL) &&
			!(query = forward_to_matching_char(
				query, token, &toklen, '"'
			)) && die_parser_error(
				"Assertion error: Unmatched char (\")",
				first
			);

			*query == '\'' &&
			(token->type = STRING_LITERAL) &&
			!(query = forward_to_matching_char(
				query, token, &toklen, '\''
			)) && die_parser_error(
				"Assertion error: Unmatched char (')",
				first
			);
			toklen++; query++;
		}
	}
	if (!token->toklen) token->toklen = toklen;
	if (!token->token && token == first) {
		free_tokens(first);
		first = NULL;
	}

	return first; 
}

int
main (int argc, char *argv []) {
	/* copy contents of file into buffer "contents" */
	char *contents;
	size_t input_size = 0;
	char *tempFile = ".dlqtemp";
	FILE *input = NULL;
	FILE *temp = NULL;
	char buf[1];
	
	if (argc > 1) {
		if(!strcmp(argv[1],"-")) {
			input = stdin;
		}
		else {
			input = fopen(argv[1],"r");
			if (NULL == input) {
				fprintf(
					stderr,
					"Unable to open '%s': %s\n",
					argv[1],
					strerror(errno)
				);
				exit(EXIT_FAILURE);
			}
		}
	}
	else {
		input = stdin;
	}

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
	Token *tokens = tokenize (contents);

	//printf("%s", contents);

	/* replace '.' notation with functional equivalent */
	tokens = normalize (tokens);

	/* create abstract syntax tree from Tokens */
//	const Expression *program = analyze (tokens);

	Token *token = tokens;
	while (token) {
		write(1, token->token, token->toklen);
		printf(" ");
		fflush(stdout);
		token = token->next;
	}
	puts("\n");

	printf("allocs(%li) tokens(%li)\n", numAllocs, numTokens);
	free_tokens (tokens);
	free (contents);
	return 0;
}
