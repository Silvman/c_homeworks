//
// Created by silvman on 15.03.18.
//

#include "stack.h"

parse_token pop(stack *p_stack) {
    parse_token tmp = peek(p_stack);
    if ( tmp.o != EMTPY_FLAG ) {
        p_stack->count--;
    }

    return tmp;
}

int is_empty(stack *p_stack) {
    return p_stack->count == 0;
}

int push(stack *p_stack, parse_token token) {
    if(p_stack->count == p_stack->buffer_size) {
        p_stack->buffer_size = expand_buffer(&(p_stack->buffer), p_stack->buffer_size);
        if (!p_stack->buffer_size) {
            return ERROR_CODE;
        }
    }

    p_stack->buffer[p_stack->count] = token;
    p_stack->count++;
    return SUCCESS_CODE;
}

parse_token peek(stack *p_stack) {
    parse_token tmp;
    if ( !is_empty(p_stack) ) {
        tmp = p_stack->buffer[p_stack->count - 1];
        return tmp;
    }

    tmp.f = 0;
    tmp.o = EMTPY_FLAG;
    return tmp;
}

int init_stack(stack *p_stack, size_t buf_size) {
    p_stack->buffer = calloc(buf_size, sizeof(parse_token));
    if (p_stack->buffer) {
        p_stack->count = 0;
        p_stack->buffer_size = buf_size;
        return SUCCESS_CODE;
    }

    return ERROR_CODE;
}

int free_stack(stack *p_stack) {
    free(p_stack->buffer);
    p_stack->count = 0;
    p_stack->buffer_size = 0;
    return 0;
}

size_t expand_buffer(parse_token **p_buffer, size_t buffer_size) {
    size_t new_size = buffer_size * 2;
//    parse_token* new_buffer = realloc(*p_buffer, sizeof(char) * new_size);

    parse_token temp_buffer[buffer_size];
    memcpy(temp_buffer, *p_buffer, buffer_size * sizeof(parse_token));
    free(*p_buffer);

    *p_buffer = calloc(new_size, sizeof(parse_token));
    if (!(*p_buffer)) {
        return 0;
    }

    memcpy(*p_buffer, temp_buffer, buffer_size * sizeof(parse_token));

    return new_size;
}

void swap(parse_token* a, parse_token* b) {
    parse_token c = *a;
    *a = *b;
    *b = c;
}

void reverse_stack(stack *p_stack) {
    for(int i = 0; i < p_stack->count / 2; i++) {
        swap(p_stack->buffer + i, p_stack->buffer + p_stack->count - 1 - i);
    }
}

int flush_stack(stack *from, stack *to) {
    while ( !is_empty(from) ) {
        if (peek(to).o == '(') {
            return ERROR_CODE;
        }

        push(to, pop(from));
    }

    return SUCCESS_CODE;
}
