#include <stdio.h>
#include <stdlib.h>

struct User{
    int ID;
    char name[50];
    int age;
};

void createUser(){
    struct User user;

    FILE *f = fopen("users.txt", "a");
    if(f == NULL){
        printf("Unable to open file\n");
        return;
    }

    printf("Enter ID :");
    scanf("%d", &user.ID);

    printf("Enter Name :");
    scanf("%49s", user.name);

    printf("Enter Age :");
    scanf("%d", &user.age);

    fprintf(f, "%d %s %d\n", user.ID, user.name, user.age);

    fclose(f);
    printf("User added successfully");
}

void readUsers(){
    struct User user;
    FILE *f = fopen("users.txt", "r");

    if(f == NULL){
        printf("No users Found.\n");
        return ;
    }

    printf("ID\tName\tAge\n");

    while(fscanf(f, "%d %s %d", &user.ID, user.name, &user.age) != EOF){
        printf("%d\t%s\t%d\n", user.ID, user.name, user.age);
    }
    fclose(f);
}

void updateUser(int ID){
    struct User user;
    FILE *f = fopen("users.txt", "r");
    FILE *temp = fopen("temp.txt", "w");

    if(f == NULL || temp == NULL){
        printf("Error opening file\n");
        return;
    }

    int found = 0;
    while(fscanf(f, "%d %s %d", &user.ID, user.name, &user.age) != EOF){
        if(user.ID == ID){
            found = 1;

            printf("Enter new Name : ");
            scanf("%s", user.name);

            printf("Enter new Age : ");
            scanf("%d", &user.age);
        }

        fprintf(temp, "%d %s %d\n", user.ID, user.name, user.age);
    }

    fclose(f);
    fclose(temp);

    if(found){
        remove("users.txt");
        rename("temp.txt", "users.txt");
        printf("User updated successfully");
    } else {
        remove("temp.txt");
        printf("User ID not found\n");
    }
}

void deleteUser(int ID){
    struct User user;
    FILE *f = fopen("users.txt", "r");
    FILE *temp = fopen("temp.txt", "w");

    if(f == NULL || temp == NULL){
        printf("Error opening file\n");
        return;
    }

    while(fscanf(f, "%d %s %d", &user.ID, user.name, &user.age) != EOF){
        if(user.ID != ID){
            fprintf(temp, "%d %s %d\n", user.ID, user.name, user.age);
        }
    }

    fclose(f);
    fclose(temp);

    remove("users.txt");
    rename("temp.txt", "users.txt");
    printf("User deleted successfully");
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
                updateUser(id);
                break;
            case 4:
                printf("Enter ID to delete: ");
                scanf("%d", &id);
                deleteUser(id);
                break;
            case 5:
                exit(0);
            default:
                printf("Invalid choice\n");
        }
    }
}