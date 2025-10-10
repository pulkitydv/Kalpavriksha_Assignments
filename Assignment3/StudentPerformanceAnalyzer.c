#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define MAX_STUDENTS 100

typedef struct Student {
    int RollNo;
    char name[50];
    int Marks1;
    int Marks2;
    int Marks3;
} Student;

int calculateTotalMarks(Student student) {
    return student.Marks1 + student.Marks2 + student.Marks3;
}

float calculateAverageMarks(int totalMarks) {
    return totalMarks / 3.0;
}

char calculateGrade(float averageMarks) {
    if (averageMarks >= 85) {
        return 'A';
    }
    else if (averageMarks >= 70) {
        return 'B';
    }
    else if (averageMarks >= 50) {
        return 'C';
    }
    else if (averageMarks >= 35) {
        return 'D';
    }
    else {
        return 'F';
    }
}

void performanceRatingByGrade(char grade) {
    int stars = 0;
    switch (grade) {
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

    for (int i = 0; i < stars; i++) {
        printf("*");
    }
    printf("\n");
}

void printRollNumbers(Student students[], int index, int totalStudents) {
    if (index == totalStudents){
        return;
    }
    printf("%d ", students[index].RollNo);
    printRollNumbers(students, index + 1, totalStudents);
}

int isRollNoDuplicate(Student students[], int currentIndex, int rollNo) {
    for (int i = 0; i < currentIndex; i++) {
        if (students[i].RollNo == rollNo) {
            return 1;
        }
    }
    return 0;
}

void displayStudentDetails(Student students[], int index) {
    int totalMarks = calculateTotalMarks(students[index]);
    float avgMarks = calculateAverageMarks(totalMarks);
    char grade = calculateGrade(avgMarks);

    printf("Roll: %d\n", students[index].RollNo);
    printf("Name: %s\n", students[index].name);
    printf("Total: %d\n", totalMarks);
    printf("Average: %.2f\n", avgMarks);
    printf("Grade: %c\n", grade);
    if (grade != 'F') {
        printf("Performance: ");
        performanceRatingByGrade(grade);
    }
    printf("\n");
}

int main() {
    int totalStudents;

    do {
        if (scanf("%d", &totalStudents) != 1) {
            printf("Invalid input . Please enter a number.\n");
            while (getchar() != '\n');
            continue;
        }
        if (totalStudents < 1 || totalStudents > 100) {
            printf("Invalid number of students. Try again.\n");
        }
    } while (totalStudents < 1 || totalStudents > 100);

    getchar();

    Student students[MAX_STUDENTS];

    for (int i = 0; i < totalStudents; i++) {
        char line[200];
        int valid = 0;

        while (!valid) {
            if (!fgets(line, sizeof(line), stdin)) {
                printf("Error in reading the input.\n");
                continue;
            }

            int roll, marks1, marks2, marks3;
            char name[50];

            int parsed = sscanf(line, "%d %49s %d %d %d", &roll, name, &marks1, &marks2, &marks3);
            if (parsed != 5) {
                printf("Invalid input format. Please type all details correctly.\n");
                continue;
            }

            if (roll <= 0) {
                printf("Roll number must be positive.\n");
                continue;
            }

            if (isRollNoDuplicate(students, i, roll)) {
                printf("Roll number already exists.\n");
                continue;
            }

            if (marks1 < 0 || marks1 > 100 || marks2 < 0 || marks2 > 100 || marks3 < 0 || marks3 > 100) {
                printf("Marks must be between 0 and 100.\n");
                continue;
            }

            students[i].RollNo = roll;
            strcpy(students[i].name, name);
            students[i].Marks1 = marks1;
            students[i].Marks2 = marks2;
            students[i].Marks3 = marks3;
            valid = 1;
        }
    }

    printf("\nOutput:\n");

    for (int i = 0; i < totalStudents; i++) {
        displayStudentDetails(students, i);
    }

    printf("List of Roll Numbers (via recursion): ");
    printRollNumbers(students, 0, totalStudents);
    printf("\n");

    return 0;
}
