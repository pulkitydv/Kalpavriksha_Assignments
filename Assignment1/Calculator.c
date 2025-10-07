#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

#define STACK_SIZE 100

int error = 0;

int isOperator(char c);
int parseNumber(const char *input, int *index, int length, int isNegative);
void stackPush(int *stack, int *top, int value);
int handleOperation(int *stack, int *top, char op, int num);
int calculateFinalResult(const int *stack, int top);
int evaluateExpression(const char *input);
void removeSpaces(char *str);

int main()
{
    char input[1000];

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

int isOperator(char c)
{
    return c == '+' || c == '-' || c == '*' || c == '/';
}

void removeSpaces(char *str)
{
    int readIndex = 0, writeIndex = 0;
    while (str[readIndex])
    {
        if (!isspace((unsigned char)str[readIndex]))
        {
            str[writeIndex++] = str[readIndex];
        }
        readIndex++;
    }
    str[writeIndex] = '\0';
}

int parseNumber(const char *expr, int *pos, int len, int negative)
{
    long long num = 0;

    while (*pos < len && isdigit((unsigned char)expr[*pos]))
    {
        num = num * 10 + (expr[*pos] - '0');

        if ((!negative && num > INT_MAX) || (negative && num > (long long)INT_MAX + 1))
        {
            printf("Error: Integer overflow detected.\n");
            error = 1;
            return 0;
        }
        (*pos)++;
    }

    if (negative) num = -num;

    if (num < INT_MIN || num > INT_MAX)
    {
        printf("Error: Number out of range.\n");
        error = 1;
        return 0;
    }

    return (int)num;
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

int handleOperation(int *stack, int *top, char op, int num)
{
    if (op == '/')
    {
        if (num == 0)
        {
            printf("Error: Division by zero.\n");
            error = 1;
            return 0;
        }
        stack[*top] /= num;
    }
    else if (op == '*')
    {
        long long res = (long long)stack[*top] * num;
        if (res > INT_MAX || res < INT_MIN)
        {
            printf("Error: Multiplication overflow.\n");
            error = 1;
            return 0;
        }
        stack[*top] = (int)res;
    }
    else if (op == '+')
    {
        stackPush(stack, top, num);
    }
    else if (op == '-')
    {
        stackPush(stack, top, -num);
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

int evaluateExpression(const char *expr)
{
    int stack[STACK_SIZE];
    int top = -1;
    int len = strlen(expr);
    int i = 0;

    char currentOp = '+';
    int expectNumber = 1;
    int negativeFlag = 0;

    while (i < len)
    {
        char c = expr[i];

        if (isspace((unsigned char)c))
        {
            i++;
            continue;
        }

        if (isdigit((unsigned char)c))
        {
            int value = parseNumber(expr, &i, len, negativeFlag);
            negativeFlag = 0;
            if (error) return 0;

            handleOperation(stack, &top, currentOp, value);
            if (error) return 0;

            expectNumber = 0;
            continue;
        }

        if (isOperator(c))
        {
            // Handle negative numbers at start or after another operator
            if (expectNumber && c == '-')
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
                printf("Error: Expression cannot start with '%c'.\n", c);
                error = 1;
                return 0;
            }

            currentOp = c;
            expectNumber = 1;
            i++;
        }
        else
        {
            printf("Error: Unexpected character '%c'.\n", c);
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
