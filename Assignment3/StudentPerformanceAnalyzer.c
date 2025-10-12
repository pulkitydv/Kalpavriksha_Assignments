#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STUDENTS 100
#define TOTAL_SUBJECTS 3

typedef struct Student
{
    int RollNo;
    char name[50];
    int marks[TOTAL_SUBJECTS];
} Student;

int calculateTotalMarks(Student student)
{
    int totalMarks = 0;
    for (int i = 0; i < TOTAL_SUBJECTS; i++)
    {
        totalMarks += student.marks[i];
    }
    return totalMarks;
}

float calculateAverageMarks(int totalMarks)
{
    return (float)totalMarks / TOTAL_SUBJECTS;
}

char calculateGrade(float averageMarks)
{
    if (averageMarks >= 85)
    {
        return 'A';
    }
    else if (averageMarks >= 70)
    {
        return 'B';
    }
    else if (averageMarks >= 50)
    {
        return 'C';
    }
    else if (averageMarks >= 35)
    {
        return 'D';
    }
    else
    {
        return 'F';
    }
}

void performanceRatingByGrade(char grade)
{
    int stars = 0;
    switch (grade)
    {
    case 'A':
        stars = 5;
        break;
    case 'B':
        stars = 4;
        break;
    case 'C':
        stars = 3;
        break;
    case 'D':
        stars = 2;
        break;
    default:
        stars = 0;
        break;
    }

    for (int i = 0; i < stars; i++)
    {
        printf("*");
    }
    printf("\n");
}

void printRollNumbers(Student students[], int index, int totalStudents)
{
    if (index == totalStudents)
    {
        return;
    }
    printf("%d ", students[index].RollNo);
    printRollNumbers(students, index + 1, totalStudents);
}

int isRollNoDuplicate(Student students[], int currentIndex, int rollNo)
{
    for (int i = 0; i < currentIndex; i++)
    {
        if (students[i].RollNo == rollNo)
        {
            return 1;
        }
    }
    return 0;
}

void displayStudentDetails(Student students[], int index)
{
    int totalMarks = calculateTotalMarks(students[index]);
    float avgMarks = calculateAverageMarks(totalMarks);
    char grade = calculateGrade(avgMarks);

    printf("Roll: %d\n", students[index].RollNo);
    printf("Name: %s\n", students[index].name);
    printf("Total: %d\n", totalMarks);
    printf("Average: %.2f\n", avgMarks);
    printf("Grade: %c\n", grade);
    if (grade != 'F')
    {
        printf("Performance: ");
        performanceRatingByGrade(grade);
    }
    printf("\n");
}

int main()
{
    int totalStudents;

    do
    {
        if (scanf("%d", &totalStudents) != 1)
        {
            printf("Invalid input. Please enter a number.\n");
            while (getchar() != '\n')
                ;
            continue;
        }
        if (totalStudents < 1 || totalStudents > 100)
        {
            printf("Invalid number of students. Try again.\n");
        }
    } while (totalStudents < 1 || totalStudents > 100);

    getchar();

    Student students[MAX_STUDENTS];

    for (int studentIndex = 0; studentIndex < totalStudents; studentIndex++)
    {
        char line[200];
        int valid = 0;

        while (!valid)
        {
            if (!fgets(line, sizeof(line), stdin))
            {
                printf("Error in reading the input.\n");
                continue;
            }

            int rollNumber;
            char name[50];
            int marks[TOTAL_SUBJECTS];

            int parsed = sscanf(line, "%d %49[^0-9] %d %d %d", &rollNumber, name, &marks[0], &marks[1], &marks[2]);
            if (parsed != 5)
            {
                printf("Invalid input format. Please type all details correctly.\n");
                continue;
            }

            if (rollNumber <= 0)
            {
                printf("Roll number must be positive.\n");
                continue;
            }

            if (isRollNoDuplicate(students, studentIndex, rollNumber))
            {
                printf("Roll number already exists. Please enter a unique roll number.\n");
                continue;
            }

            if (marks[0] < 0 || marks[0] > 100 || marks[1] < 0 || marks[1] > 100 || marks[2] < 0 || marks[2] > 100)
            {
                printf("Marks must be between 0 and 100. Please enter valid marks.\n");
                continue;
            }

            students[studentIndex].RollNo = rollNumber;
            strcpy(students[studentIndex].name, name);

            for (int subjectNumber = 0; subjectNumber < TOTAL_SUBJECTS; subjectNumber++)
            {
                students[studentIndex].marks[subjectNumber] = marks[subjectNumber];
            }
            valid = 1;
        }
    }

    printf("\nOutput:\n");

    for (int i = 0; i < totalStudents; i++)
    {
        displayStudentDetails(students, i);
    }

    printf("List of Roll Numbers (via recursion): ");
    printRollNumbers(students, 0, totalStudents);
    printf("\n");

    return 0;
}
