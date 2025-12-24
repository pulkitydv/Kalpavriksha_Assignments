#include <stdio.h>

int main()
{
    int elements;

    printf("Enter number of elements: ");
    if (scanf("%d", &elements) != 1 || elements <= 0)
    {
        printf("Error: Invalid number of elements.\n");
        return 1;
    }

    int array[elements];
    printf("Enter array elements: ");
    for (int index = 0; index < elements; index++)
    {
        if (scanf("%d", array + index) != 1)
        {
            printf("Error: Invalid input.\n");
            return 1;
        }
    }

    int *pointer = array;
    int *lastNonZero = array;

    for (int index = 0; index < elements; index++)
    {
        if (*(pointer + index) != 0)
        {
            *(lastNonZero) = *(pointer + index);
            lastNonZero++;
        }
    }

    while (lastNonZero < array + elements)
    {
        *lastNonZero = 0;
        lastNonZero++;
    }

    printf("Array after moving zeros: ");
    for (int index = 0; index < elements; index++)
    {
        printf("%d ", *(array + index));
    }
    printf("\n");
    return 0;
}
