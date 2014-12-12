#include "DLQParser.h"

void
die_parser_error(const char *msg, Token *token)
{
  free_tokens(token);
  fprintf(stderr, "Parser error: %s\n", msg);
  exit(1);
}

int
assert_matching_char(const char *query, Token *token, size_t *toklen, char c)
{
  int tl = *toklen;
  if (*query == c) {
    query++; tl++;
    while (*query != c) {
      if (*query == '\0') {
        return 1;
      }
      tl++; query++;
    }
    token->toklen = ++tl;
    *toklen = 0;
  }
  return 0;
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
    tptr = allocs[numAllocs++] = malloc(sizeof(Token) * BUFFSIZE * ++multiplier);
    numTokens = 0;
  }
  tptr->token = NULL;
  tptr->toklen = 0;
  tptr->managed = false;
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
  while (numAllocs--) {
    free(allocs[numAllocs]);
  }
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
  size_t toklen = strlen(str);
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
  return is_numeric(*(dot + 1)) || (check_behind && is_numeric(*(dot - 1)));
}

Token *
normalize(Token *tokens)
{
  Token *expression, *remove, *next = tokens;
  int ins = 0;

  while (next) {
    if (*next->token == '.' && next->toklen == 1) {

      if (next == tokens || !next->next || *next->next->token == '.') {
        /* 
         * can't start query with '.';
         * can't have hanging '.' at the end;
         * can't have two '.'s in a row;
         * */
        Token *bad = next;
        char *t; int i = 10;
        while(i-- && bad->prev) bad = next->prev;
        i = 10;
        while (i-- && bad != next) {
          size_t toklen = bad->toklen;
          while (toklen--) write (2, bad->token++, 1);
        }
        die_parser_error("illegal use of '.' token.", tokens);
      }

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
        insert_tokens_after(next, token_from_char('('));
        ins = insert_tokens_after(next->next, expression);
        while (--ins) expression = expression->next;
        insert_tokens_after(expression, token_from_char(')'));
      }
    }
    else next = next->next;
  }
  return tokens;
}

Token *
tokenize(const char *query)
{
  size_t toklen = 0;
  bool stop = false;
  Token *first, *prev, *token;
  const char *orig = query;

  allocs[numAllocs++] = tptr = malloc(sizeof(Token) * BUFFSIZE * multiplier);
  token = first = new_token();

  token->token = query;

  /* tokenize char[] by creating Tokens whose token->token point to the start of
   * each token and provide the length in token->toklen. THESE ARE NOT "C STRINGS" */
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
      if (*query == '.' && is_float_dot(query, orig != query)) {
        query++; toklen++;
        break;
      }
      if (!token->toklen) token->toklen = toklen;

      token = inc_token(token);
      token->token = query++;
      token->toklen = 1;

      toklen = 0;
      break;
    case SPACE:
      if (!token->toklen) token->toklen = toklen; toklen = 0;
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
      if (token->toklen) {
        token = inc_token(token);
        token->token = query;
      }
      if (assert_matching_char(query, token, &toklen, '"'))
        die_parser_error("Assertion error: Unmatched char (\")", first);
      if (assert_matching_char(query, token, &toklen, '\''))
        die_parser_error("Assertion error: Unmatched char (')", first);
      toklen++; query++;
    }
  }
  if (!token->toklen) token->toklen = toklen;

  return first; 
}

size_t
getSizeOfInput(FILE *input){
   size_t retvalue = 0;
   char c;

   if (input != stdin) {
      if (-1 == fseek(input, 0L, SEEK_END)) {
         fprintf(stderr, "Error seek end: %s\n", strerror(errno));
         exit(EXIT_FAILURE);
      }
      if (-1 == (retvalue = ftell(input))) {
         fprintf(stderr, "ftell failed: %s\n", strerror(errno));
         exit(EXIT_FAILURE);
      }
      if (-1 == fseek(input, 0L, SEEK_SET)) {
         fprintf(stderr, "Error seek start: %s\n", strerror(errno));
         exit(EXIT_FAILURE);
      }
   } else {
      /* for stdin, we need to read in the entire stream until EOF */
      while (EOF != (c = fgetc(input))) {
         retvalue++;
      }
   }

   return retvalue;
}

int
main (int argc, char *argv []) {
  /* copy contents of file into buffer "contents" */
  char *contents;
  size_t input_size;
  char *tempFile = ".dlqtemp";
  FILE *input;
  FILE *temp = fopen(tempFile, "w");
  char buf[1];
  
  if (argc > 1) {
    if(!strcmp(argv[1],"-")) {
      input = stdin;
    } else {
      input = fopen(argv[1],"r");
      if (NULL == input) {
        fprintf(stderr, "Unable to open '%s': %s\n",
              argv[1], strerror(errno));
        exit(EXIT_FAILURE);
      }
    }
  } else {
    input = stdin;
  }

  while (EOF != (buf[0] = fgetc(input))) {
    fwrite(buf, sizeof(char), 1, temp);
  }
  fclose(temp);
  fclose(input);

  temp = fopen(tempFile, "r");
  input_size = getSizeOfInput(temp);

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

  Token *token = tokens;
  while (token) {
    write(1, token->token, token->toklen);
    printf(" ");
    fflush(stdout);
    token = token->next;
  }
  puts("\n");

  free (contents);
  printf("allocs(%li) tokens(%li)\n", numAllocs, numTokens);
  free_tokens (tokens);
  return 0;
}
