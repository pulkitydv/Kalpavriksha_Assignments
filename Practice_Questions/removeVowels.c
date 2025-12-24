#include<stdio.h>

#define STRING_SIZE 100
#define ERROR 0
#define SUCCESS 1

void trimNewLine(char *string) {    
    int currentIndex = 0;

    while(*(string + currentIndex) != '\0') {
        if (*(string + currentIndex) == '\n') {
            *(string + currentIndex) = '\0';
            break;
        }
        currentIndex++;
    }
}

void removeVowels(char *string) {
    char *source = string;
    char *destination = string;

    while (*source != '\0')
    {
        char ch = *source;
        if (ch != 'a' && ch != 'e' && ch != 'i' && ch != 'o' && ch != 'u' &&
            ch != 'A' && ch != 'E' && ch != 'I' && ch != 'O' && ch != 'U')
        {
            *destination = *source;
            destination++;
        }
        source++;
    }

    *destination = '\0';
}

int validString(char *string)
{
    int currentIndex = 0;
    int isValid = 1;

    if (*string == '\0')
    {
        printf("Error: Empty string.\n");
        return ERROR;
    }
    else
    {
        while (*(string + currentIndex) != '\0')
        {
            char ch = *(string + currentIndex);
            if (!((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') ||
                  ch == ' ' || ch == '\t'))
            {
                printf("Error: Invalid characters in string.\n");
                isValid = 0;
                break;
            }
            currentIndex++;
        }
    }

    return isValid ? SUCCESS : ERROR;
}

void printString(char *string) {
    char *pointer = string;
    int isEmpty = 1;

    while(*pointer != '\0') {
        if(*pointer != ' ' && *pointer != '\t'){
            isEmpty = 0;
            break;
        }
        pointer++;
    }

    if (isEmpty) {
        printf("String is empty or contains only whitespace characters.\n");
    }
    else {
        printf("String after removing vowels : %s\n", string);
    }
}

void removeLeadingSpaces(char *string) {
    char *source = string;
    
    while(*source == ' ' || *source == '\t') {
        source++;
    }

    char *destination = string;
    while (*source != '\0')
    {
        *destination = *source;
        destination++;
        source++;
    }
    *destination = '\0';
}

int main(){
    char string[STRING_SIZE];

    printf("Enter a string: ");
    fgets(string, sizeof(string), stdin);
    trimNewLine(string);
    removeLeadingSpaces(string);
    if (!validString(string)) {
        return 1;
    }
    removeVowels(string);
    printString(string);
    return 0;
}
