#include <stdio.h>

#define ERROR 0
#define SUCCESS 1 
#define STRING_SIZE 100

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

void removeLeadingSpaces(char *string)
{
    char *source = string;

    while (*source == ' ' || *source == '\t')
    {
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

void mystrcpy(char *inputString, char *copiedString){

    char *inputStringPointer = inputString;
    char *copiedStringPointer = copiedString;

    while (*inputStringPointer != '\0') {
        *copiedStringPointer = *inputStringPointer;
        inputStringPointer++;
        copiedStringPointer++;
    }
    *copiedStringPointer = '\0';
}

int main(){
    
    char inputString[STRING_SIZE];
    char copiedString[STRING_SIZE];

    printf("Source : ");
    fgets(inputString, STRING_SIZE, stdin);
    trimNewLine(inputString);
    removeLeadingSpaces(inputString);
    if (!validString(inputString)){
        return 1;
    }

    mystrcpy(inputString, copiedString);
    printf("Copied String : %s\n", copiedString);
}
