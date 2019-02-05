//
// Created by silvman on 15.03.18.
//

#ifndef CALC_PARSER_H
#define CALC_PARSER_H

#include <stdio.h>
#include "stack.h"

#define EMTPY_FLAG      'e'

typedef struct parse_token {
    float f;
    char o;
} parse_token;

typedef struct stack stack;

enum return_statements {
    DO_NOT_PUSH_CODE = -2,
    ERROR_CODE = -1,
    SUCCESS_CODE = 0
};

int parse(stack* p_stack, stack* p_tmp_stack);
int parse_float(int *char_buffer, parse_token *p_token);
int handle_operation(char c, parse_token *p_token, stack* p_result_stack, stack* p_temp_stack);

int get_num(char symbol);
int get_priority(char symbol);
int is_operation(char symbol);
int is_special_symbol(char symbol);

float calculate(stack* expression);
parse_token make_operation(stack* p_tmp_stack, parse_token token);

#endif //CALC_PARSER_H
