#ifndef TOKENS_H
#define TOKENS_H
#include "strarr.h"

char* read_string(const char* text, int i);

strarr* get_tokens(const char* text);

int isoperator(int i);

int isarg(int i);

#endif
