#include <stdio.h>
#include "strarr.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "tokens.h"


// Read the given string starting at i. Only includes characters
//  that are included in the current token.
char*
read_string(const char* text, int i) 
{
  // bytes = how long the token is
  int bytes = 1;
  // if it's an & or a |, check if the next char is the same
  if (text[i] == '&' || text[i] == '|') {
    while(text[i+bytes] == text[i]) {
      bytes++;
    }
  }

  else if (isarg(text[i])) {
    while (isarg(text[i+bytes])) {
      bytes++;
    }
  }      

  char* string = (char*)malloc((bytes + 1) * sizeof(char));
  // null terminate the string
  string[bytes] = 0;
  memcpy(string, text + i, bytes);
  return string;
}

strarr*
get_tokens(const char* text)
{
  strarr *tokens = create_strarr();

  int i = 0;
  while (i < strlen(text)) {
    // if this character is a space, ignore it
    if (isspace(text[i])) {
      i++;
      continue;
    }
    // if this character is an operator
    else if (isoperator(text[i])) {
      char* op = read_string(text, i);
      strarr_add(tokens, op);
      i += (int)strlen(op);
      free(op);
      continue;  
    }
    // if this character is a command argument
    else if (isarg(text[i])) {
      char* an = read_string(text, i);
      strarr_add(tokens, an);
      i += strlen(an);
      free(an);
      continue; 
    }
  }
  return tokens;
}

// Determines if the given character is a special operator.
//  Operators are '<', '>', '|', '&', and ';'.
int
isoperator(int i)
{
  if (i == '<' || i == '>' || i == '|' || i == '&' || i == ';') { 
    return 1;
  }
  return 0;
} 

// Determines if the given character is a command argument. 
//  Arguments are the alphanumeric characters, '-', '.', '/', and '_'.
int
isarg(int i) 
{
  if (isalnum(i)) {
    return 1;
  }
  else if (i == '-' || i == '.' || i == '/' || i == '_') {
    return 1;
  }
  return 0;
}

/*
// Run the tokenizer.
int
n(int _argc, char* _argv[])
{
  char line[100];
  while(1) {
    char* rv = fgets(line, 96, stdin);
    if (!rv) {
      break;
    }
    // print out the tokens (individual elements in strarr)
    get_tokens(rv);
  }   
}
*/
