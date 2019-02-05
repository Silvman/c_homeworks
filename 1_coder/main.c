#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

// 2 <= Q < P < 36
#define MIN_BASE 2
#define MAX_BASE 36
#define BUF_SIZE 4
#define INPUT_BUF_SIZE 128
#define DEC_BASE 10
#define ERROR_CODE (-1)

// двойной указатель - потому что realloc может переместить на другой адрес
// Производит чтение с stdin и запускает функцию перевода в систему счисления
// Возвращает NULL в случае неудачи
char *work(char** input, int* base_from, int* base_to);

// Функция для чтения чисел P и Q
int scanf_num();

// Функция для чтения числа в изначальной системе счисления
char** scanf_string(char** input);

// Возвращает десятичную цифру
int get_num(char symbol);

// Возвращает символ по десятичному представлению
char get_symbol(int num);

// Функция перевода в нужную систему счисления
// Возвращает NULL в случае неудачи
char* translate_to_base(const char *input, const int *base_from, const int *base_to);

// Переводит число в стороковом представлении в long
// Возвращает -1 в случае ошибки
long get_lnum_from_string(const char *input, int base_from);

// Увеличение буфера под строку, перезаписывает указатель
// Возвращает размер нового буфера, buffer_size - старый размер буфера
// 0 - если ошибка
size_t expand_buffer(char **buffer, size_t buffer_size);

// Переворачивает строку. Нужна в translate_to_base, чтобы число было записано в верном порядке.
void reverse(char* string);
void swap(char* a, char* b);

// Проверяет, чтобы в строковом представлении числа не было лишних символов
int check_correct(const char* input);

//  на вход P Q S
int main() {
    char* input = calloc(INPUT_BUF_SIZE, sizeof(char));
    char* result = NULL;
    int* base_from = malloc(sizeof(int)); // попросили разместить в динамической памяти
    int* base_to = malloc(sizeof(int)); // попросили разместить в динамической памяти

    if (input && base_from && base_to) {
        result = work(&input, base_from, base_to);
        if (!result) {
            printf("[error]");
        } else {
            printf("%s", result);
        }
    }

    if (input)
        free(input);

    if (base_from)
        free(base_from);

    if (base_to)
        free(base_to);

    if (result)
        free(result);

    return 0;
}



char* work(char** input, int* base_from, int* base_to) {
    *base_from = scanf_num();
    *base_to = scanf_num();
    if (!scanf_string(input)) {
        return NULL;
    }

    if (    check_correct(*input) == ERROR_CODE ||
            *base_from == ERROR_CODE || *base_to == ERROR_CODE ||
            *base_to < MIN_BASE || *base_to >= *base_from || *base_from > MAX_BASE) {
        return NULL;
    }

    return translate_to_base(*input, base_from, base_to);
}

// получить десятичное представление
// while'ом получить все десятичные представления символов
// переводить их в символы и собирать строку
// вернуть строку
char* translate_to_base(const char *input, const int *base_from, const int *base_to) {
    long decimal = get_lnum_from_string(input, *base_from);
    if (decimal == ERROR_CODE)
        return NULL;

    size_t count = 0;
    size_t buffer_size = BUF_SIZE;
    char* buffer = calloc(buffer_size, sizeof(char));

    if(!buffer) {
        return NULL;
    }

    do {
        buffer[count++] = get_symbol((int)(decimal % *base_to));
        decimal /= *base_to;

        if (count >= buffer_size) {
            buffer_size = expand_buffer(&buffer, buffer_size);
            if (!buffer_size)
                return NULL;
        }
    } while (decimal != 0);

    buffer[count] = '\0';

    // число записывалось в обратном порядке
    reverse(buffer);
    return buffer;
}

long get_lnum_from_string(const char *input, int base_from) {
    int dec;
    long result = 0;

    for(int i = 0; i < strlen(input); i++) {
        dec = get_num(input[i]);

        if (dec != ERROR_CODE && dec < base_from) {
            result = result * base_from + get_num(input[i]);
        } else {
            return ERROR_CODE;
        }
    }

    return result;
}

int get_num(char symbol) {
    if (isdigit(symbol)) {
        return symbol - '0';
    }

    // Используем в качестве цифр буквы латинского алфавита
    if (islower(symbol)) {
        return symbol - 'a' + DEC_BASE;
    }

    if (isupper(symbol)) {
        return symbol - 'A' + DEC_BASE;
    }

    // если что-то пошло не так
    return ERROR_CODE;
}

char get_symbol(const int num) {
    if (num >= 0 && num < DEC_BASE) {
        // '0' - для сдвига на коды цифр в ASCII кодировке
        return (char)(num + '0');
    }

    if (num >= DEC_BASE && num <= MAX_BASE) {
        // 'A' - для сдвига на коды заглавных букв в ASCII кодировке
        return (char)(num - DEC_BASE + 'A');
    }

    // если что-то пошло не так
    return '\0';
}

void swap(char *a, char *b) {
    char c = *a;
    *a = *b;
    *b = c;
}

void reverse(char *string) {
        for(int i = 0; i < strlen(string) / 2; i++) {
            swap(string + i, string + strlen(string) - 1 - i);
            // -1 - по причине что первый элемент считается с 0, а strlen возвращает количество (считает с 1)
        }
}

int check_correct(const char *input) {
    for (int i = 0; i < strlen(input); i++) {
        if( !isalnum(input[i]) ) {
            return ERROR_CODE;
        }
    }
    return 0;
}

size_t expand_buffer(char **buffer, size_t buffer_size) {
    size_t new_size = buffer_size * 2;
    char* new_buffer = realloc(*buffer, sizeof(char) * new_size);

    if (!new_buffer) {
        free(*buffer);
        return 0;
    }

    if (new_buffer != *buffer) {
        *buffer = new_buffer;
    }

    return new_size;
}

int scanf_num() {
    char buf[3]; // максимум 2 символа + нуль-символ

    if (scanf("%2s", buf) == 1) {
        while ( !isspace(getchar()) ) {  }
        return (int) get_lnum_from_string(buf, DEC_BASE);
    } else {
        return 0;
    }
}

char** scanf_string(char** input) {
    // буфер для последовательного чтения строки
    size_t buf_size = INPUT_BUF_SIZE;
    char input_buf[buf_size];

    // 127 = INPUT_BUF_SIZE - 1
    if (scanf("%127s", input_buf) == 1) {
        strncpy(*input, input_buf, strlen(input_buf) + 1); // с нуль-символом
        int c;
        while ( (c = getchar()) && !(isspace(c) || iscntrl(c) || c == EOF) ) {
            // 126 = 127 - 1, т.к. getchar
            if ( scanf("%126s", input_buf) == 1 ) {
                size_t length = strlen(*input);
                buf_size = expand_buffer(input, buf_size);
                if (!buf_size)
                    return NULL;

                // не забываем про пробный символ
                (*input)[length] = (char) c;

                // + 1 - с учетом пробного символа
                strncpy(*input + length + 1, input_buf, strlen(input_buf) + 1);
            }
        }
    } else {
        return NULL;
    }

    return input;
}