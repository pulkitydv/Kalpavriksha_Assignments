#include <stdio.h>
#include <stdlib.h>

#define USER_FILE "users.txt"
#define TEMP_FILE "temp.txt"

struct User{
    int ID;
    char name[50];
    int age;
};

void createUser(){
    struct User user;

    FILE *file = fopen(USER_FILE, "a");
    if(file == NULL){
        printf("Unable to open file\n");
        return;
    }

    printf("Enter ID :");
    scanf("%d", &user.ID);

    printf("Enter Name :");
    scanf("%49s", user.name);

    printf("Enter Age :");
    scanf("%d", &user.age);

    fprintf(file, "%d %s %d\n", user.ID, user.name, user.age);

    fclose(file);
    printf("User added successfully");
}

void readUsers(){
    struct User user;
    FILE *file = fopen(USER_FILE, "r");

    if(file == NULL){
        printf("No users Found.\n");
        return ;
    }

    printf("ID\tName\tAge\n");

    while(fscanf(file, "%d %s %d", &user.ID, user.name, &user.age) != EOF){
        printf("%d\t%s\t%d\n", user.ID, user.name, user.age);
    }
    fclose(file);
}

void updateUserById(int ID){
    struct User user;
    FILE *file = fopen(USER_FILE, "r");
    FILE *temp = fopen(TEMP_FILE, "w");

    if(file == NULL || temp == NULL){
        printf("Error opening file\n");
        return;
    }

    int found = 0;
    while(fscanf(file, "%d %s %d", &user.ID, user.name, &user.age) != EOF){
        if(user.ID == ID){
            found = 1;

            printf("Enter new Name : ");
            scanf("%s", user.name);

            printf("Enter new Age : ");
            scanf("%d", &user.age);
        }

        fprintf(temp, "%d %s %d\n", user.ID, user.name, user.age);
    }

    fclose(file);
    fclose(temp);

    if(found){
        remove(USER_FILE);
        rename(TEMP_FILE, USER_FILE);
        printf("User updated successfully");
    } else {
        remove(TEMP_FILE);
        printf("User ID not found\n");
    }
}

void deleteUserById(int ID){
    struct User user;
    FILE *file = fopen(USER_FILE, "r");
    FILE *temp = fopen(TEMP_FILE, "w");

    if(file == NULL || temp == NULL){
        printf("Error opening file\n");
        return;
    }

    int found = 0;
    while(fscanf(file, "%d %s %d", &user.ID, user.name, &user.age) != EOF){
        if(user.ID != ID)
        {
            fprintf(temp, "%d %s %d\n", user.ID, user.name, user.age);
        } 
        else 
        {
            found = 1;
        }
    }

    fclose(file);
    fclose(temp);

    remove(USER_FILE);
    rename(TEMP_FILE, USER_FILE);

    if(!found){
        printf("User ID not found\n");
    }
    else
    {
        printf("User deleted successfully");
    }
}

int main() {
    int choice, id;

    while (1) {
        printf("\nMenu:\n");
        printf("1. Add User\n");
        printf("2. Display Users\n");
        printf("3. Update User\n");
        printf("4. Delete User\n");
        printf("5. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        switch(choice) {
            case 1:
                createUser();
                break;
            case 2:
                readUsers();
                break;
            case 3:
                printf("Enter ID to update: ");
                scanf("%d", &id);
                updateUserById(id);
                break;
            case 4:
                printf("Enter ID to delete: ");
                scanf("%d", &id);
                deleteUserById(id);
                break;
            case 5:
                printf("Exiting...\n");
                return 0;
            default:
                printf("Invalid choice\n");
        }
    }
}
