#include<stdio.h>

void sumAlternateElements(int *array, int size, int *sum){
    int *pointer = array;
    *sum = 0;

    for (int count = 0; count < (size + 1) / 2; count++)
    {
        *sum += *pointer;
        pointer += 2;
    }
}

int main(){
    int elements;
    printf("Enter number of elements : ");
    if(scanf("%d", &elements) != 1) {
        printf("Error: Invalid input.\n");
        return 1;
    }

    if(elements <= 0) {
        printf("Error: Number of elements must be positive.\n");
        return 1;
    }

    int array[elements];
    printf("Enter elements: ");
    for (int index = 0; index < elements; index++)
    {
        if (scanf("%d", array + index) != 1)
        {
            printf("Error: Invalid input.\n");
            return 1;
        }
    }

    int sum;
    sumAlternateElements(array, elements, &sum);

    printf("Sum of alternate elements: %d\n", sum);
    return 0;
}
