#ifndef STRARR_H
#define STRARR_H

typedef struct strarr {
  int size;
  int capacity;
  char** data;
} strarr;

strarr* create_strarr();
void free_strarr(strarr* sv);

char* strarr_get(strarr* str, int ii);
void strarr_put(strarr* str, int ii, char* item);

void strarr_add(strarr* str, char* item);

#endif
