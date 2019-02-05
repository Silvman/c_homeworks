//
// Created by silvman on 15.03.18.
//

#ifndef CALC_STACK_H
#define CALC_STACK_H

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"

#define BUFFER_SIZE     3

typedef struct parse_token parse_token;

typedef struct stack {
    parse_token* buffer;
    size_t count; // он же top + 1
    size_t buffer_size;
} stack;


int init_stack(stack* p_stack, size_t buf_size);
int free_stack(stack* p_stack);

parse_token peek(stack* p_stack);
parse_token pop(stack* p_stack);
int push(stack* p_stack, parse_token token);
size_t expand_buffer(parse_token** p_buffer, size_t buffer_size);
int flush_stack(stack* from, stack* to);

void reverse_stack(stack* p_stack);
int is_empty(stack* p_stack);
void swap(parse_token* a, parse_token* b);

#endif //CALC_STACK_H
