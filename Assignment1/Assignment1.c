#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LEN 100

void removeSpaces(char* str) {
    int i, j = 0;
    for (i = 0; str[i]; i++) {
        if (!isspace(str[i])) {
            str[j++] = str[i];
        }
    }
    str[j] = '\0';
}

int isValidExpression(const char* expr) {
    for (int i = 0; expr[i]; i++) {
        if (!isdigit(expr[i]) && expr[i] != '+' && expr[i] != '-' &&
            expr[i] != '*' && expr[i] != '/') {
            return 0;
        }
    }
    return 1;
}

int main() {
    char input[MAX_LEN];

    printf("Enter expression: ");
    if (!fgets(input, sizeof(input), stdin)) {
        printf("Error: Invalid input.\n");
        return 0;
    }

    input[strcspn(input, "\n")] = '\0';

    removeSpaces(input);

    if (!isValidExpression(input)) {
        printf("Error: Invalid expression.\n");
        return 0;
    }

    int numbers[MAX_LEN];
    char ops[MAX_LEN];
    int numCount = 0, opCount = 0;

    int i = 0;
    while (input[i]) {
        if (isdigit(input[i])) {
            int val = 0;
            while (isdigit(input[i])) {
                val = val * 10 + (input[i] - '0');
                i++;
            }
            numbers[numCount++] = val;
        } else {
            ops[opCount++] = input[i];
            i++;
        }
    }

    for (i = 0; i < opCount; i++) {
        if (ops[i] == '*' || ops[i] == '/') {
            int a = numbers[i];
            int b = numbers[i + 1];

            if (ops[i] == '/') {
                if (b == 0) {
                    printf("Error: Division by zero.\n");
                    return 0;
                }
                numbers[i] = a / b;
            } else {
                numbers[i] = a * b;
            }

            for (int j = i + 1; j < numCount - 1; j++)
                numbers[j] = numbers[j + 1];

            for (int j = i; j < opCount - 1; j++)
                ops[j] = ops[j + 1];

            numCount--;
            opCount--;
            i--;
        }
    }

    int result = numbers[0];
    for (i = 0; i < opCount; i++) {
        if (ops[i] == '+')
            result += numbers[i + 1];
        else if (ops[i] == '-')
            result -= numbers[i + 1];
    }

    printf("%d\n", result);
    return 0;
}