#include <stdio.h>
#include <stdlib.h>

#define MAX_WORD_LENGTH 50

void readWords(int numberOfWords, char words[][MAX_WORD_LENGTH + 1])
{
    printf("Enter words:\n");
    for (int word = 0; word < numberOfWords; word++)
    {
        scanf("%s", *(words + word));
    }
}

int wordLength(char *word)
{
    int wordLength = 0;
    while (*(word + wordLength) != '\0')
    {
        wordLength++;
    }
    return wordLength;
}

char *findLongestWord(int numberOfWords, char words[][MAX_WORD_LENGTH + 1], int *maxLength)
{
    char *longestWord = *words;
    *maxLength = 0;

    for (int word = 0; word < numberOfWords; word++)
    {
        char *currentWord = *(words + word);
        int length = wordLength(currentWord);

        if (length > *maxLength)
        {
            *maxLength = length;
            longestWord = currentWord;
        }
    }

    return longestWord;
}

int main()
{
    int numberOfWords;

    printf("Enter number of words: ");
    if (scanf("%d", &numberOfWords) != 1 || numberOfWords <= 0)
    {
        printf("Error: Invalid number of words.\n");
        return 1;
    }

    char words[numberOfWords][MAX_WORD_LENGTH + 1];

    readWords(numberOfWords, words);

    int maxLength;
    char *longestWord = findLongestWord(numberOfWords, words, &maxLength);

    printf("The longest word is: %s\n", longestWord);
    printf("Length: %d\n", maxLength);

    return 0;
}
