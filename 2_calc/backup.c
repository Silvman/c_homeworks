#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE     3
#define EMTPY_FLAG      'e'

typedef struct parse_token {
    float f;
    char o;
} parse_token;


enum return_statements {
    DO_NOT_PUSH_CODE = -2,
    ERROR_CODE = -1,
    SUCCESS_CODE = 0
};


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

int parse(stack* p_stack, stack* p_tmp_stack);
int parse_float(int *char_buffer, parse_token *p_token);
int handle_operation(char c, parse_token *p_token, stack* p_result_stack, stack* p_temp_stack);

int get_num(char symbol);
int get_priority(char symbol);
int is_operation(char symbol);
int is_special_symbol(char symbol);

float calculate(stack* expression);
parse_token make_operation(stack* p_tmp_stack, parse_token token);





int main() {
    stack stack, tmp_stack;
    int init_code_stack      = init_stack(&stack, BUFFER_SIZE);
    int init_code_temp_stack = init_stack(&tmp_stack, BUFFER_SIZE);

    if (  init_code_stack           == SUCCESS_CODE &&
          init_code_temp_stack      == SUCCESS_CODE &&
          parse(&stack, &tmp_stack) == SUCCESS_CODE) {
        float result = calculate(&stack);
        printf("%.2f\n", result);
    } else {
        printf("[error]\n");
    }

    if (init_code_stack == SUCCESS_CODE)
        free_stack(&stack);

    if (init_code_temp_stack == SUCCESS_CODE)
        free_stack(&tmp_stack);

    return 0;
}

int get_num(char symbol) {
    if (isdigit(symbol)) {
        return symbol - '0';
    }

    // если что-то пошло не так
    return ERROR_CODE;
}

parse_token pop(stack *p_stack) {
    parse_token tmp = peek(p_stack);
    if ( tmp.o != EMTPY_FLAG ) {
        p_stack->count--;
    }

    return tmp;
}

int is_empty(stack *p_stack) {
    if ( p_stack->count ) {
        return 0;
    }

    return 1;
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

int parse(stack *p_stack, stack *p_tmp_stack) {
    int unary_minus_possible = 1;
    int c;
    while ( (c = getchar()) && !(c == EOF || c == '\n') ) {
        if (isspace(c))
            continue;

        parse_token tmp = {0, 0};
        if (isdigit(c) || (unary_minus_possible && c == '-') ) {
            int parse_code = parse_float(&c, &tmp);
            if (unary_minus_possible)
                unary_minus_possible = 0;

            if (parse_code == ERROR_CODE || push(p_stack, tmp) == ERROR_CODE || c == ERROR_CODE)
                return ERROR_CODE;

            if (c == '\0')
                continue;
        }

        if (is_operation((char)c))
            unary_minus_possible = 1;

        int state = handle_operation((char)c, &tmp, p_stack, p_tmp_stack);
        switch (state) {
            case ERROR_CODE:
                return ERROR_CODE; // посторонний символ в строке

            case DO_NOT_PUSH_CODE:
                continue;

            case SUCCESS_CODE:
            default:
                if(push(p_tmp_stack, tmp) == ERROR_CODE)
                    return ERROR_CODE;

                continue;
        }
    }

    if (flush_stack(p_tmp_stack, p_stack) == ERROR_CODE) {
        return ERROR_CODE;
    }

    reverse_stack(p_stack);
    return SUCCESS_CODE;
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

int parse_float(int *char_buffer, parse_token *p_token) {
    int is_unary_minus = 0;
    int divide = 0; // "знаменатель"
    int expect_number = 0;

    if ( *char_buffer == '-' ) {
        is_unary_minus = 1;

        *char_buffer = getchar();
        if(!isdigit(*char_buffer))
            return ERROR_CODE;
    }

    p_token->f = get_num((char) *char_buffer); // "числитель"
    int c;
    while ((c = getchar()) && (isdigit(c) || c == '.')) {
        if (divide) {
            expect_number = 0;
            divide       *= 10;
        }

        if (c != '.') {
            p_token->f = p_token->f * 10 + get_num((char) c);
        } else {
            // если встретили вторую точку
            if (divide != 0)
                return ERROR_CODE;

            expect_number   = 1;
            divide          = 1;
        }
    }

    if (expect_number)
        return ERROR_CODE; // точку встретили, но числа не дождались

    if (divide)
        p_token->f /= divide;

    if (is_unary_minus)
        p_token->f = -p_token->f;

    if (is_special_symbol( (char)c )){
        // помещаем в буффер, чтобы после продолжить парсинг
        *char_buffer = c;
    } else {
        if (isspace(c) || c == EOF)
            *char_buffer = '\0';
        else
            *char_buffer = ERROR_CODE;
    }

    return SUCCESS_CODE;
}

int handle_operation(char c, parse_token *p_token, stack *p_result_stack, stack *p_temp_stack) {
    p_token->f = 0;

    char b;
    switch(c) {
        case '+':
        case '-':
        case '/':
        case '*':
            while( (b = peek(p_temp_stack).o) && (is_operation(b)) && (get_priority(c) <= get_priority(b)) ) {
                push(p_result_stack, pop(p_temp_stack));
            }

            p_token->o = c;
            break;

        case '(':
            p_token->o = c;
            break;

        case ')':
            while( (b = peek(p_temp_stack).o) && b != '(' && b != EMTPY_FLAG) {
                push(p_result_stack, pop(p_temp_stack));
            }

            if(peek(p_temp_stack).o == '(') {
                pop(p_temp_stack);
                return DO_NOT_PUSH_CODE; // не нужно отправлять токен в стек
            }
            return ERROR_CODE; // пропущена скобка

        default:
            return ERROR_CODE;
    }
    return SUCCESS_CODE;
}

parse_token make_operation(stack* p_tmp_stack, parse_token token) {
    parse_token tmp = {0, 0};
    float temp_float;

    switch (token.o) {
        case '\0':
            tmp = token;
            break;

        case '+':
            tmp.f = pop(p_tmp_stack).f + pop(p_tmp_stack).f;
            break;

        case '-':
            temp_float = pop(p_tmp_stack).f;
            tmp.f = pop(p_tmp_stack).f - temp_float;
            break;

        case '*':
            tmp.f = pop(p_tmp_stack).f * pop(p_tmp_stack).f;
            break;

        case '/':
            temp_float = pop(p_tmp_stack).f;
            tmp.f = pop(p_tmp_stack).f / temp_float;
            break;

        case EMTPY_FLAG:
        default:
            tmp.o = EMTPY_FLAG;
            break;
    }

    return tmp;
}

float calculate(stack *expression) {
    float end_result = 0;

    stack tmp_stack;
    int init_code = init_stack(&tmp_stack, BUFFER_SIZE); // обработать

    if (init_code == SUCCESS_CODE) {
        while ( !is_empty(expression) ) {
            parse_token token = pop(expression);
            parse_token result = make_operation(&tmp_stack, token);

            if (result.o != EMTPY_FLAG) {
                push(&tmp_stack, result);
            } else {
                // произошла ошибка
                free_stack(&tmp_stack);
                return end_result;
            }
        }

        end_result = pop(&tmp_stack).f;
        free_stack(&tmp_stack);
    }

    return end_result;
}

int get_priority(char symbol) {
    switch (symbol) {
        case '+':
        case '-':
            return 1;

        case '*':
        case '/':
            return 2;

        case '(':
        case ')':
            return 0;

        default:
            break;
    }

    return ERROR_CODE;
}

int is_operation(char symbol) {
    switch (symbol) {
        case '+':
        case '-':
        case '/':
        case '*':
            return 1;

        default:
            return 0;
    }
}

int is_special_symbol(char symbol) {
    return (is_operation(symbol) || symbol == '(' || symbol == ')');
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
