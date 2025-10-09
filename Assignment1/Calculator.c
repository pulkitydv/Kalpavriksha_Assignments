#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

#define STACK_SIZE 100

int error = 0;

int isOperator(char character);
int parseNumber(const char *input, int *index, int length, int isNegative);
void stackPush(int *stack, int *top, int value);
int handleOperation(int *stack, int *top, char operator, int number);
int calculateFinalResult(const int *stack, int top);
int evaluateExpression(const char *input);
void removeSpaces(char *expression);

int main()
{
    char input[100];

    fgets(input, sizeof(input), stdin);
    error = 0;

    removeSpaces(input);

    int result = evaluateExpression(input);
    if (!error)
    {
        printf("%d\n", result);
    }

    return 0;
}

int isOperator(char character)
{
    return character == '+' || character == '-' || character == '*' || character == '/';
}

void removeSpaces(char *expression)
{
    int readIndex = 0, writeIndex = 0;
    while (expression[readIndex])
    {
        if (!isspace((unsigned char)expression[readIndex]))
        {
            expression[writeIndex++] = expression[readIndex];
        }
        readIndex++;
    }
    expression[writeIndex] = '\0';
}

int parseNumber(const char *expression, int *index, int length, int isNegative)
{
    long long number = 0;

    while (*index < length && isdigit((unsigned char)expression[*index]))
    {
        number = number * 10 + (expression[*index] - '0');

        if ((!isNegative && number > INT_MAX) || (isNegative && number > (long long)INT_MAX + 1))
        {
            printf("Error: Integer overflow detected.\n");
            error = 1;
            return 0;
        }
        (*index)++;
    }

    if (isNegative) number = -number;

    if (number < INT_MIN || number > INT_MAX)
    {
        printf("Error: Number out of range.\n");
        error = 1;
        return 0;
    }

    return (int)number;
}

void stackPush(int *stack, int *top, int value)
{
    if (*top >= STACK_SIZE - 1)
    {
        printf("Error: Stack is full.\n");
        error = 1;
        return;
    }
    stack[++(*top)] = value;
}

int handleOperation(int *stack, int *top, char operator, int number)
{
    if (operator == '/')
    {
        if (number == 0)
        {
            printf("Error: Division by zero.\n");
            error = 1;
            return 0;
        }
        stack[*top] /= number;
    }
    else if (operator == '*')
    {
        long long result = (long long)stack[*top] * number;
        if (result > INT_MAX || result < INT_MIN)
        {
            printf("Error: Multiplication overflow.\n");
            error = 1;
            return 0;
        }
        stack[*top] = (int)result;
    }
    else if (operator == '+')
    {
        stackPush(stack, top, number);
    }
    else if (operator == '-')
    {
        stackPush(stack, top, -number);
    }
    return 1;
}

int calculateFinalResult(const int *stack, int top)
{
    long long total = 0;
    for (int i = 0; i <= top; i++)
    {
        total += stack[i];
        if (total > INT_MAX || total < INT_MIN)
        {
            printf("Error: Final result overflow.\n");
            error = 1;
            return 0;
        }
    }
    return (int)total;
}

int evaluateExpression(const char *expression)
{
    int stack[STACK_SIZE];
    int top = -1;
    int length = strlen(expression);
    int i = 0;

    char currentOperator = '+';
    int expectNumber = 1;
    int negativeFlag = 0;

    while (i < length)
    {
        char character = expression[i];

        if (isspace((unsigned char)character))
        {
            i++;
            continue;
        }

        if (isdigit((unsigned char)character))
        {
            int value = parseNumber(expression, &i, length, negativeFlag);
            negativeFlag = 0;
            if (error) return 0;

            handleOperation(stack, &top, currentOperator, value);
            if (error) return 0;

            expectNumber = 0;
            continue;
        }

        if (isOperator(character))
        {
            // Handle negative numbers at start or after another operator
            if (expectNumber && character == '-')
            {
                if (negativeFlag)
                {
                    printf("Error: Invalid '--' sequence.\n");
                    error = 1;
                    return 0;
                }
                negativeFlag = 1;
                i++;
                continue;
            }

            if (expectNumber)
            {
                printf("Error: Expression cannot start with '%character'.\n", character);
                error = 1;
                return 0;
            }

            currentOperator = character;
            expectNumber = 1;
            i++;
        }
        else
        {
            printf("Error: Unexpected character '%character'.\n", character);
            error = 1;
            return 0;
        }
    }

    if (expectNumber)
    {
        printf("Error: Expression cannot end with an operator.\n");
        error = 1;
        return 0;
    }

    if (top == -1)
    {
        printf("Error: No numbers found.\n");
        error = 1;
        return 0;
    }

    return calculateFinalResult(stack, top);
}
