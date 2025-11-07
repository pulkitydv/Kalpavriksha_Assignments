#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define BLOCKS_PER_FILE 100
#define LINE_LENGTH 80
#define MAX_BLOCK_SIZE 512
#define MAX_NAME_LENGTH 51
#define MAX_NUM_BLOCKS 1024
#define PATH_SIZE 1000

typedef struct FreeBlock{
    int index;
    struct FreeBlock* prev;
    struct FreeBlock* next;
} FreeBlock;

typedef struct FileNode{
    char name[MAX_NAME_LENGTH];
    int isDirectory;
    struct FileNode* parent;
    struct FileNode* next;
    struct FileNode* child;
    int blockPointers[BLOCKS_PER_FILE];
    int contentSize;
    int numOfBlocks;
} FileNode;

char virtualDisk[MAX_NUM_BLOCKS][MAX_BLOCK_SIZE];
FreeBlock* freeListHead = NULL;
FileNode* root = NULL;
FileNode* cwd = NULL;

void initializeFreeBlock(){
    FreeBlock* previous = NULL;

    for(int index = 0; index < MAX_NUM_BLOCKS; index++){
        FreeBlock* block = (FreeBlock*) malloc(sizeof(FreeBlock));
        block->index = index;
        block->next = NULL;
        block->prev = previous;
        
        if(previous){
            previous->next = block;
        }
        else {
            freeListHead = block;
        }

        previous = block;
    }
}

void initializeFileSystem() {
    initializeFreeBlock();

    root = (FileNode*) malloc(sizeof(FileNode));
    root->isDirectory = 1;
    root->parent = NULL;
    root->child = NULL;
    strcpy(root->name, "/");
    root->next = root;
    root->contentSize = 0;
    root->numOfBlocks = 0;
    cwd = root;
    printf("Compact VFS - ready. Type 'exit' to quit.\n");
}

int allocateFreeBlock(){
    if(freeListHead == NULL){
        printf("No memory left.\n");
        return -1;
    }
    FreeBlock* allocatedBlock = freeListHead;
    int index = allocatedBlock->index;
    freeListHead = freeListHead->next;
    if(freeListHead){
        freeListHead->prev = NULL;
    }
    free(allocatedBlock);
    return index;
}

void freeFileBlocks(FileNode* file){
    for(int index = 0; index < file->numOfBlocks; index++){
        int blockIndex = file->blockPointers[index];
        if(blockIndex >= 0){
            FreeBlock* newFreeBlock = (FreeBlock*)malloc(sizeof(FreeBlock));
            newFreeBlock->index = blockIndex;
            newFreeBlock->next = freeListHead;
            if(freeListHead){
                freeListHead->prev = newFreeBlock;
            }
            newFreeBlock->prev = NULL;
            freeListHead = newFreeBlock;
            file->blockPointers[index] = -1;
        }
    }
    file->contentSize = 0;
    file->numOfBlocks = 0;
}

void mkdir(char* name){
    FileNode* node = cwd->child;
    if(node){
        do{
            if(strcmp(node->name, name) == 0){
                printf("Directory with name %s already exists.\n", name);
                return;
            }
            node = node->next;
        } while (node != cwd->child);
    }
    FileNode* newDirectory = (FileNode*) malloc(sizeof(FileNode));
    strcpy(newDirectory->name, name);
    newDirectory->isDirectory = 1;
    newDirectory->parent = cwd;
    newDirectory->child = NULL;
    newDirectory->contentSize = 0;
    newDirectory->numOfBlocks = 0;
    if(cwd->child == NULL){
        cwd->child = newDirectory;
        newDirectory->next = newDirectory;
    } 
    else{
        FileNode* temp = cwd->child;
        while (temp->next != cwd->child){
            temp = temp->next;
        }        
        newDirectory->next = temp->next;
        temp->next = newDirectory;
    }
    printf("Directory '%s' created successfully.\n", name);
}

void ls(){
    FileNode* temp = cwd->child;
    if(temp == NULL){
        printf("(empty)\n");
        return;
    }
    do{
        printf("%s", temp->name);
        if(temp->isDirectory){
            printf("/");
        }
        printf("\n");
        temp = temp->next;
    }while(temp != cwd->child);
}

void pwd()
{
    FileNode *temp = cwd;
    char path[PATH_SIZE] = "";
    char tempPath[PATH_SIZE] = "";
    while (temp != root)
    {
        snprintf(tempPath, sizeof(tempPath), "%s/%s", temp->name, path);
        strncpy(path, tempPath, sizeof(path));
        temp = temp->parent;
    }
    if (strlen(path) == 0)
    {
        printf("/\n");
        return;
    }
    printf("/%s\n", path);
}

void cdParent(){
    if (cwd == root)
    {
        printf("Already on root.\n");
        return;
    }
    cwd = cwd->parent;
    printf("Moved to ");
    pwd();
}

void cd(char* name){

    if(strcmp(name, "..") == 0){
        cdParent();
    }
    else{
        if(cwd->child == NULL){
            printf("%s is empty.\n", cwd->name);
            return;
        }
        FileNode* temp = cwd->child;
        do{
            if(strcmp(temp->name, name) == 0){
                if(temp->isDirectory == 1){
                    cwd = temp;
                    printf("Moved to ");
                    pwd();
                    return;
                }
            }
            temp = temp->next;
        } while (temp != cwd->child);
        printf("No folder found with the name %s\n", name);
    }
}

void create(char* name)
{
    FileNode *node = cwd->child;
    if (node)
    {
        do
        {
            if (strcmp(node->name, name) == 0)
            {
                printf("File with name %s already exists.\n", name);
                return;
            }
            node = node->next;
        } while (node != cwd->child);
    }
    FileNode *newFile = (FileNode *)malloc(sizeof(FileNode));
    strcpy(newFile->name, name);
    newFile->isDirectory = 0;
    newFile->parent = cwd;
    newFile->child = NULL;
    newFile->contentSize = 0;
    newFile->numOfBlocks = 0;
    for(int index = 0; index < BLOCKS_PER_FILE; index++){
        *(newFile->blockPointers + index) = -1;
    }
    if (cwd->child == NULL)
    {
        cwd->child = newFile;
        newFile->next = newFile;
    }
    else
    {
        FileNode *temp = cwd->child;
        while (temp->next != cwd->child)
        {
            temp = temp->next;
        }
        newFile->next = temp->next;
        temp->next = newFile;
    }
    printf("File '%s' created successfully.\n", name);
}

void writeData(FileNode* file, char* data){
    if(!file || file->isDirectory){
        printf("Invalid file.\n");
        return;
    }
    if(file->numOfBlocks > 0){
        freeFileBlocks(file);
    }
    int dataLength = strlen(data);
    int requiredBlocks = (strlen(data) + MAX_BLOCK_SIZE - 1) / MAX_BLOCK_SIZE;
    file->contentSize = strlen(data);
    file->numOfBlocks = 0;
    int start = 0;
    for(int index = 0; index < requiredBlocks; index++){
        int blockIndex = allocateFreeBlock();
        if(blockIndex == -1){
            return;
        }
        file->blockPointers[file->numOfBlocks] = blockIndex;
        file->numOfBlocks++;
        strncpy(virtualDisk[blockIndex], data + start, MAX_BLOCK_SIZE);
        start += MAX_BLOCK_SIZE;
    }
    printf("Data written successfully(size = %d bytes)\n", file->contentSize);
}

void write(char* fileName, char* data){
    if(data[0] != '"' || data[strlen(data) - 1] != '"'){
        printf("Syntax error.\n");
        return;
    }
    data[strlen(data) - 1] = '\0';
    data++;
    int index2 = 0;
    for(int index = 0; data[index] != '\0'; index++, index2++){
        if(data[index] == '\\' && data[index+1] == 'n'){
            data[index2] = '\n';
            index++;
        } else {
            data[index2] = data[index];
        }
    }
    data[index2] = '\0';

    FileNode* temp = cwd->child;
    if(temp == NULL){
        printf("No file found.\n");
        return;
    }
    do{
        if(strcmp(temp->name, fileName) == 0){
            writeData(temp, data);
            return;
        }
        temp = temp->next;
    } while(temp != cwd->child);
    printf("No file found with name %s.\n", fileName);
}

void readData(FileNode* file){
    if(file->numOfBlocks == 0){
        printf("File is empty.\n");
        return;
    }
    for(int index = 0; index < file->numOfBlocks; index++){
        int blockIndex = file->blockPointers[index];
        if(blockIndex >= 0){
            printf("%s", virtualDisk[blockIndex]);
        }
    }
    printf("\n");
}

void read(char* fileName){
    FileNode* temp = cwd->child;
    if(!temp){
        printf("No file found.\n");
        return;
    }
    do{
        if(strcmp(temp->name, fileName) == 0){
            if(!temp->isDirectory){
                readData(temp);
                return;
            }
        }
        temp = temp->next;
    } while(temp != cwd->child);
    printf("No file found with name %s.\n", fileName);
}

void deleteFile(FileNode* file){
    freeFileBlocks(file);
    if(file->next == file){
        cwd->child = NULL;
    }
    else{
        FileNode* temp = cwd->child;
        while(temp->next != file && temp->next != cwd->child){
            temp = temp->next;
        }
        temp->next = file->next;
        if(file == cwd->child){
            cwd->child = file->next;
        }
    }
    free(file);
    printf("File deleted successfully.\n");
}

void delete(char* fileName){
    if(cwd->child == NULL){
        printf("No file found.\n");
        return;
    }
    FileNode* temp = cwd->child;
    do{
        if(strcmp(temp->name, fileName) == 0){
            if(!temp->isDirectory){
                deleteFile(temp);
                return;
            }
        }
        temp = temp->next;
    } while(temp != cwd->child);
    printf("No file found with name %s.\n", fileName);
}

void rmdir(char* dirName){
    if(cwd->child == NULL){
        printf("No directory found.\n");
        return;
    }
    FileNode* temp = cwd->child;
    do{
        if(strcmp(temp->name, dirName) == 0){
            if(!temp->isDirectory){
                printf("%s is not a directory.\n", dirName);
                return;
            }
            if(temp->child != NULL){
                printf("Directory not empty. Remove files first.\n");
                return;
            }
            if(temp->next == temp){
                cwd->child = NULL;
            }
            else{
                FileNode* node = cwd->child;
                while(node->next != temp && node->next != cwd->child){
                    node = node->next;
                }
                node->next = temp->next;
                if(temp == cwd->child){
                    cwd->child = temp->next;
                }
            }
            free(temp);
            printf("Directory removed successfully.\n");
            return;
        }
        temp = temp->next;
    } while(temp != cwd->child);
    printf("No directory found with name %s.\n", dirName);
}

void df(){
    int freeBlocks = 0;
    FreeBlock* temp = freeListHead;
    while(temp){
        freeBlocks++;
        temp = temp->next;
    }
    printf("Total blocks: %d\n", MAX_NUM_BLOCKS);
    printf("Used blocks: %d\n", MAX_NUM_BLOCKS - freeBlocks);
    printf("Free blocks: %d\n", freeBlocks);
    printf("Disk usage: %.2f%%\n", (float)((MAX_NUM_BLOCKS - freeBlocks) * 100.0) / MAX_NUM_BLOCKS);
}

void freeFreeBlocks()
{
    FreeBlock *temp = freeListHead;
    while (temp)
    {
        FreeBlock *next = temp->next;
        free(temp);
        temp = next;
    }
    freeListHead = NULL;
}

void freeFileNodes(FileNode *node)
{
    if (!node)
        return;
    FileNode *child = node->child;
    if (child)
    {
        FileNode *start = child;
        do
        {
            FileNode *nextChild = child->next;
            freeFileNodes(child);
            child = (nextChild == start) ? NULL : nextChild;
        } while (child);
    }
    if (node->isDirectory == 0)
    {
        freeFileBlocks(node);
    }
    free(node);
}

void exit_system()
{
    freeFileNodes(root);
    root = NULL;
    cwd = NULL;
    freeFreeBlocks();
    printf("Memory released. Exiting program...\n");
}

void takeInput(){
    printf("%s > ", cwd->name);
    char line[LINE_LENGTH];
    fgets(line, LINE_LENGTH, stdin);
    line[strcspn(line, "\n")] = '\0';

    char* command = strtok(line, " ");
    if(command == NULL){
        return;
    }
    if(strcmp(command, "mkdir") == 0){
        char *argument = strtok(NULL, " ");
        if(argument == NULL){
            printf("Write the name of the directory.\n");
            return;
        }
        mkdir(argument);
    }
    else if(strcmp(command, "cd") == 0){
        char *argument = strtok(NULL, " ");
        if (argument == NULL){
            printf("Write the name of the directory.\n");
            return;
        }
        cd(argument);
    }
    else if(strcmp(command, "cd..") == 0){
        cdParent();
    }
    else if(strcmp(command, "ls") == 0){
        ls();
    } 
    else if(strcmp(command, "create") == 0){
        char* argument = strtok(NULL, " ");
        if(argument == NULL){
            printf("Write the name of the file.\n");
            return;
        }
        create(argument);
    }
    else if(strcmp(command, "write") == 0){
        char* filename = strtok(NULL, " ");
        char* data = strtok(NULL, "\n");
        if(!filename || !data){
            printf("Syntax: write filename \"data\"\n");
            return;
        }
        write(filename, data);
    }
    else if(strcmp(command, "read") == 0){
        char* filename = strtok(NULL, " ");
        if(!filename){
            printf("Write the name of the file.\n");
            return;
        }
        read(filename);
    }
    else if(strcmp(command, "delete") == 0){
        char* filename = strtok(NULL, " ");
        if(filename == NULL){
            printf("Write the name of the file to delete.\n");
            return;
        }
        delete(filename);
    }
    else if(strcmp(command, "rmdir") == 0){
        char* argument = strtok(NULL, " ");
        if(argument == NULL){
            printf("Write the name of the directory to remove.\n");
            return;
        }
        rmdir(argument);
    }
    else if(strcmp(command, "pwd") == 0){
        pwd();
    }
    else if(strcmp(command, "df") == 0){
        df();
    }
    else if(strcmp(command, "exit") == 0){
        exit_system();
        exit(0);
    }
    else{
        printf("Invalid command.\n");
    }
}

int main(){
    initializeFileSystem();
    while (1)
    {
        takeInput();
    }
    return 0;
}
