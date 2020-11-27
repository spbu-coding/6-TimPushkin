#pragma once

#include <string.h>

#define  MAX_INPUT_STRING_SIZE 1000

typedef int (*comparator_func_t)(const char*, const char*);
typedef char** strings_array_t;
typedef size_t array_size_t;


void bubble(strings_array_t, array_size_t, comparator_func_t);


void insertion(strings_array_t, array_size_t, comparator_func_t);


void merge(strings_array_t, array_size_t, comparator_func_t);


void quick(strings_array_t, array_size_t, comparator_func_t);


void radix(strings_array_t, array_size_t, comparator_func_t);

