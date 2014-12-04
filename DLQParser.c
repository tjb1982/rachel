#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define bool int
#define true 1
#define false 0
#define cases

typedef struct Token {
  const char *token;
  int toklen;
  bool managed;
  struct Token *next;
  struct Token *prev;
} Token;

void
free_tokens (Token *tokens)
{
  Token *prev;
  while (tokens) {
    if (tokens->managed == true)
      free((void *)tokens->token);
    prev = tokens;
    tokens = tokens->next;
    free(prev);
  }
}

Token *
prior_expression(Token *token)
{
  int och = 0;
  switch (*token->prev->token) {
  case '}':
    while (token->prev && *token->prev->token != '{')
      token = token->prev;
      return prior_expression(token->prev);
      break;
  case ')':
    och = 1;
    token = token->prev->prev;
    while (token && och) {
      if (*token->token == ')')
        och++;
      else if (*token->token == '(')
        och--;
      token = token->prev;
    }
    break;
  default:
    token = token->prev;
  }
  return token;
}

void
unlink_token(Token *token)
{
  token->next->prev = 
  token->prev->next = NULL;
  token->next = token->prev = NULL;
  free_tokens(token);
}

Token *
new_token()
{
  Token *token = malloc(sizeof(Token));
  token->token = NULL;
  token->toklen = 0;
  token->managed = false;
  token->next = token->prev = NULL;
  return token;
}

Token *
token_from_string(const char *str)
{
  Token *token = new_token();
  size_t toklen = strlen(str);
  char *t = malloc(sizeof(char) * toklen + 1);
  memcpy(t, str, toklen); t[toklen] = '\0';
  token->token = t;
  token->toklen = toklen;
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

/**
 * Replace '.' notation with functional equivalent
 *
 **/
Token *
normalize(Token *tokens)
{
  Token *expression, *remove, *next = tokens;
  int ins = 0;

  while (next) {
    if (*next->token == '.') {
      expression = prior_expression(next);
      remove = next;
      next = next->next;
      if (expression == tokens) tokens = next; // reset the starting position
      unlink_token(remove);
      next->prev = expression->prev; // could be NULL
      if (next->prev) next->prev->next = next;

      if (next->next && *next->next->token == '(') {
        insert_tokens_after(next->next, expression);
      }
      else {
        insert_tokens_after(next, token_from_string("("));
        ins = insert_tokens_after(next->next, expression);
        while (--ins) expression = expression->next;
        insert_tokens_after(expression, token_from_string(")"));
      }

    }
    else next = next->next;
  }
  return tokens;
}

int
count_syntactics(const char *ptr)
{
  int syntactic_count = 0;

  while (*ptr != '\0') {
    switch (*ptr++) {
    case '(': case ')': case '[': case ']':
    case '{': case '}': case '.':
      syntactic_count++;
    default:
      break;
    }
  }
  return syntactic_count;
}

Token *
token_list(const char *eptr, int token_count)
{
  Token *token, *prev, *orig = token = new_token();
  int toklen = 0;

  while (token_count--) {
    toklen = strlen (eptr);
    token->token = eptr;
    token->toklen = toklen;
    if (token_count) {
      token->next = new_token();
      prev = token;
      token = token->next;
      token->prev = prev;
    }
    else token->next = NULL;
    eptr += toklen + 1;
  }
  return orig;
}

Token *
tokenize(char *dest, const char *query)
{
  int token_count = 0;
  char c, *orig = dest;

  /* tokenize char[] by surrounding each token with '\0' and count the number of tokens */
  while (*query != '\0') {
    switch (*query) {
    case ';':
      /* comments */
      while(*(query++ +1) != '\n')
        ;
      break;
    case '.':
      /* don't tokenize floats */
      c = *(query + 1);
      if (c >= 0x30 && c <= 0x39) {
        *dest++ = *query++;
        break;
      }
    case '(': case ')':
    case '[': case ']':
    case '{': case '}':
      if (*(dest -1) != '\0') {
        *dest++ = '\0';
        token_count++;
      }
      *dest++ = *query++;
      *dest++ = '\0';
      token_count++;
      break;
    case '\r':
      break;
    case ' ': case '\n': case ',':
      if (*(dest -1) != '\0') {
        *dest++ = '\0';
        token_count++;
      }
      while (*query == ' ' || *query == '\n' || *query == ',') {
        query++;
      }
      break;
    default:
      *dest++ = *query++;
    }
  }
  *dest = '\0';

  return token_list(orig, token_count);
}

int
main (int argc, char *argv []) {
  /* copy contents of file into buffer "contents" */
  char *contents;
  size_t input_size;
  FILE *input = fopen (argv [1], "rb");

  fseek (input, 0, SEEK_END);
  input_size = ftell (input);
  rewind (input);
  contents = malloc (
    input_size * sizeof(char) + 1
  );
  fread (
    contents,
    sizeof (char),
    input_size,
    input
  );
  contents[sizeof(char) * input_size] = '\0';
  fclose (input);

  /* allocate memory for query + nulls around syntactic chars */
  char expanded[(input_size + (count_syntactics(contents) * 2) + 1) * sizeof(char)];

  /* create doubly linked list of tokens */
  Token *tokens = tokenize (expanded, contents);

  printf("%s", contents);
  free (contents);

  /* replace '.' notation with functional equivalent */
  tokens = normalize (tokens);

  Token *token = tokens;
  while (token) {
    printf("%s ", token->token);
    token = token->next;
  }
  puts("\n");

  free_tokens (tokens);
}
