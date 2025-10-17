#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void swapValues(int *firstValue, int *secondValue)
{
    int temporary = *firstValue;
    *firstValue = *secondValue;
    *secondValue = temporary;
}

int getMatrixSize()
{
    int matrixSize, validInput;
    do
    {
        printf("Enter matrix size (2-10): ");
        validInput = scanf("%d", &matrixSize);
        if (validInput != 1)
        {
            printf("Invalid input! Please enter an integer value.\n");
            while (getchar() != '\n');
            continue;
        }
        if (matrixSize < 2 || matrixSize > 10)
        {
            printf("Invalid size! Enter a value between 2 and 10.\n");
        }
    } while (validInput != 1 || matrixSize < 2 || matrixSize > 10);

    return matrixSize;
}

void generateRandomMatrix(int *matrix, int matrixSize)
{
    srand(time(0));
    for (int row = 0; row < matrixSize; row++)
    {
        for (int column = 0; column < matrixSize; column++)
        {
            *(matrix + row * matrixSize + column) = rand() % 256;
        }
    }
}

void printMatrix(int *matrix, int matrixSize)
{
    for (int row = 0; row < matrixSize; row++)
    {
        for (int column = 0; column < matrixSize; column++)
        {
            printf("%4d ", *(matrix + row * matrixSize + column));
        }
        printf("\n");
    }
}

void rotateMatrix(int *matrix, int matrixSize)
{
    for (int row = 0; row < matrixSize; row++)
    {
        for (int column = row + 1; column < matrixSize; column++)
        {
            swapValues((matrix + row * matrixSize + column), (matrix + column * matrixSize + row));
        }
    }

    for (int row = 0; row < matrixSize; row++)
    {
        for (int column = 0; column < matrixSize / 2; column++)
        {
            swapValues((matrix + row * matrixSize + column), (matrix + row * matrixSize + (matrixSize - 1 - column)));
        }
    }
}

void smoothenMatrix(int *matrix, int matrixSize)
{
    for (int row = 0; row < matrixSize; row++)
    {
        for (int column = 0; column < matrixSize; column++)
        {
            int sum = 0, count = 0;
            for (int rowNumber = row - 1; rowNumber <= row + 1; rowNumber++)
            {
                for (int columnNumber = column - 1; columnNumber <= column + 1; columnNumber++)
                {
                    if (rowNumber >= 0 && rowNumber < matrixSize && columnNumber >= 0 && columnNumber < matrixSize)
                    {
                        sum += (*(matrix + rowNumber * matrixSize + columnNumber)) % 256;
                        count++;
                    }
                }
            }
            *(matrix + row * matrixSize + column) += (sum / count) * 256;
        }
    }

    for (int row = 0; row < matrixSize; row++)
    {
        for (int column = 0; column < matrixSize; column++)
        {
            *(matrix + row * matrixSize + column) /= 256;
        }
    }
}

int main()
{
    int matrixSize = getMatrixSize();
    int matrix[matrixSize][matrixSize];

    generateRandomMatrix(&matrix[0][0], matrixSize);

    printf("\nOriginal Randomly Generated Matrix:\n");
    printMatrix(&matrix[0][0], matrixSize);

    rotateMatrix(&matrix[0][0], matrixSize);
    printf("\nMatrix after 90° Clockwise Rotation:\n");
    printMatrix(&matrix[0][0], matrixSize);

    smoothenMatrix(&matrix[0][0], matrixSize);
    printf("\nMatrix after Applying 3×3 Smoothing Filter:\n");
    printMatrix(&matrix[0][0], matrixSize);

    return 0;
}
