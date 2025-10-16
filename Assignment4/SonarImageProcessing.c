#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main()
{
    int matrixSize;
    int validInput;
    do
    {
        validInput = scanf("%d", &matrixSize);

        if (validInput != 1)
        {
            printf("Invalid input! Please enter an integer value.\n");
            while (getchar() != '\n');
            continue;
        }

        if (matrixSize < 2 || matrixSize > 10)
        {
            printf("Invalid Matrix size! Please re-enter the size between 2 and 10 only.\n");
        }
    } while (validInput != 1 || matrixSize < 2 || matrixSize > 10);

    int matrix[matrixSize][matrixSize];

    // Generating random values for the matrix between 0 and 255
    srand(time(0));

    for (int row = 0; row < matrixSize; row++)
    {
        for (int column = 0; column < matrixSize; column++)
        {
            *(*(matrix + row) + column) = rand() % 256;
        }
    }

    printf("Original Matrix \n\n");
    for (int row = 0; row < matrixSize; row++)
    {
        for (int column = 0; column < matrixSize; column++)
        {
            printf("%4d ", *(*(matrix + row) + column));
        }
        printf("\n");
    }


    // Rotating the matrix 90 degrees clockwise
    // First step - Transpose the matrix
    for (int row = 0; row < matrixSize; row++)
    {
        for (int column = row + 1; column < matrixSize; column++)
        {
            int valueToBeSwapped = *(*(matrix + row) + column);
            *(*(matrix + row) + column) = *(*(matrix + column) + row);
            *(*(matrix + column) + row) = valueToBeSwapped;
        }
    }

    // Second step - Reverse each row
    for (int row = 0; row < matrixSize; row++)
    {
        for (int column = 0; column < matrixSize / 2; column++)
        {
            int valueToBeSwapped = *(*(matrix + row) + column);
            *(*(matrix + row) + column) = *(*(matrix + row) + (matrixSize - 1 - column));
            *(*(matrix + row) + (matrixSize - 1 - column)) = valueToBeSwapped;
        }
    }

    printf("Rotated Matrix \n\n");
    for (int row = 0; row < matrixSize; row++)
    {
        for (int column = 0; column < matrixSize; column++)
        {
            printf("%4d ", *(*(matrix + row) + column));
        }
        printf("\n");
    }

    // Smoothening part
    for (int row = 0; row < matrixSize; row++)
    {
        for (int column = 0; column < matrixSize; column++)
        {
            int sum = 0;
            int count = 0;

            for (int rowNumber = row - 1; rowNumber <= row + 1; rowNumber++)
            {
                for (int columnNumber = column - 1; columnNumber <= column + 1; columnNumber++)
                {
                    if (rowNumber >= 0 && rowNumber < matrixSize && columnNumber >= 0 && columnNumber < matrixSize)
                    {
                        sum += *(*(matrix + rowNumber) + columnNumber) % 256;
                        count++;
                    }
                }
            }

            *(*(matrix + row) + column) += (sum / count) * 256;
        }
    }

    for (int row = 0; row < matrixSize; row++)
    {
        for (int column = 0; column < matrixSize; column++)
        {
            *(*(matrix + row) + column) /= 256;
        }
    }

    printf("Output matrix \n\n");

    for (int row = 0; row < matrixSize; row++)
    {
        for (int column = 0; column < matrixSize; column++)
        {
            printf("%4d ", *(*(matrix + row) + column));
        }
        printf("\n");
    }

    return 0;
}