#include <stdio.h>
#include "stack.h"

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