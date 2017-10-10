#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include "strarr.h"

// Initialize a new strarr.
strarr*
create_strarr() 
{
  strarr* str = malloc(sizeof(strarr));
  // default size 0
  str->size = 0;
  // default capacity 4
  str->capacity = 4;
  // data initialized to the true size of the capacity
  str->data = malloc(4 * sizeof(char*));
  return str;
}

// Free all memory allocated by the given strarr.
void
free_strarr(strarr* str) 
{
  int i = 0;
  for (i = 0; i < str->size; ++i) {
    if (str->data[i] != 0) {
      free(str->data[i]); 
    }
  }
  free(str->data);
  free(str);
}

// Get the ith element of the given strarr.
char*
strarr_get(strarr* str, int i) 
{
  return str->data[i];
}
// Add the given item to the given strarr at index i.
void
strarr_put(strarr* str, int i, char* item)
{
  str->data[i] = strdup(item);
}

// Add the given item to the given strarr, and if there is not enough
//  room in the strarr, make more room. 
// NOTE: This is the function that should be called when trying to add new 
//  data to a strarr, not strarr_put, in case there isn't room left for new data.
void
strarr_add(strarr* str, char* item)
{
  if (str->size >= str->capacity) {
    str->capacity *= 2;
    str->data = (char**) realloc(str->data, str->capacity * sizeof(char*));
  }

  str->size += 1;
  strarr_put(str, str->size - 1, item);
}
