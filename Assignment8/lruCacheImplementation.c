#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define MAX_CAPACITY 1000
#define MAX_COMMAND_SIZE 150
#define MAX_VALUE_SIZE 100

typedef struct QueueNode{
    int key;
    char* value;
    struct QueueNode* prev;
    struct QueueNode* next;
} QueueNode;

typedef struct HashMapNode{
    int key;
    QueueNode* queuePtr;
} HashMapNode;

QueueNode* front = NULL;
QueueNode* rear = NULL;

HashMapNode* lruCache = NULL;
int cacheCapacity = 0;
int cacheSize = 0;

int stringCompare(char* string1, char* string2){
    while(*string1 && *string2){
        if(*string1 < *string2) return -1;
        else if(*string1 > *string2) return 1;
        string1++;
        string2++;
    }
    if(*string1) return 1;
    else if(*string2) return -1;
    else return 0;
}

char* stringCopy(char* source){
    char* destination = (char*) malloc(strlen(source) + 1);
    char* ptr = destination;
    while (*source){
        *ptr = *source;
        ptr++;
        source++;
    }
    *ptr = '\0';
    return destination;
}

int isDigit(char ch){
    return (ch >= '0' && ch <= '9');
}

int stringToInt(char* string, int* result){
    if(!string || *string == '\0') return 0;
    int isNegative = 0;
    if(*string == '-'){
        isNegative = 1;
        string++;
    }
    if(*string == '\0') return 0;
    int num = 0;
    while(*string){
        if(!isDigit(*string)){
            return 0;
        } 
        num = num * 10 + (*string - '0');
        string++;
    }
    *result =  isNegative ? -num : num;
    return 1;
}

int hashValue(int key){
    return (key % cacheCapacity); 
}

QueueNode* createQueueNode(int key, char* value){
    QueueNode* newNode = (QueueNode*) malloc(sizeof(QueueNode));
    if(!newNode){
        printf("Memory allocation failed.\n");
        return NULL;
    }
    newNode->key = key;
    newNode->value = stringCopy(value);
    newNode->next = NULL;
    newNode->prev = NULL;
    return newNode;
}

void enqueue(QueueNode* newNode){
    if(front == NULL){
        front = newNode;
        rear = newNode;
        return;
    }
    newNode->next = front;
    front->prev = newNode;
    front = newNode;
}

void removeQueueNode(QueueNode* node, int freeNode){
    if(!node){
        return;
    } 
    if(front == rear){
        front = NULL;
        rear = NULL;
        if(freeNode){
            free(node->value);
            free(node);
        }
        return;
    }
    if(node == front){
        front = front->next;
        front->prev = NULL;
        if (freeNode){
            free(node->value);
            free(node);
        }
        return;
    }
    if(node == rear){
        rear->prev->next = NULL;
        rear = rear->prev;
        if (freeNode){
            free(node->value);
            free(node);
        }
        return;
    }
    node->prev->next = node->next;
    node->next->prev = node->prev;
    if (freeNode){
        free(node->value);
        free(node);
    }
}

int hashMapSearch(int key){
    int index = hashValue(key);
    for(int i = 0; i < cacheCapacity; i++){
        int try = (index + i) % cacheCapacity;
        if(lruCache[try].queuePtr == NULL){
            return -1;
        }
        if(lruCache[try].key == key){
            return try; 
        }
    }
    return -1;
}

void hashMapInsert(int key, QueueNode* queuePtr){
    int index = hashValue(key);
    for(int i = 0; i < cacheCapacity; i++){
        int try = (index + i) % cacheCapacity;
        if(lruCache[try].queuePtr == NULL){
            lruCache[try].key = key;
            lruCache[try].queuePtr = queuePtr;
            return;
        }
    }
}

void hashMapReplace(int oldKey, int newKey, QueueNode* queuePtr){
    int index = hashMapSearch(oldKey);
    lruCache[index].key = newKey;
    lruCache[index].queuePtr = queuePtr;
}

void createCache(){
    lruCache = (HashMapNode*) malloc(cacheCapacity * sizeof(HashMapNode));
    for(int i = 0; i < cacheCapacity; i++){
        (lruCache + i)->queuePtr = NULL;
    }
}

void handleCreateCache(char* command, int* cacheCreated, int* capacity){
    if(*cacheCreated){
        printf("Cache already created.\n");
        return;
    }
    char capacityStr[10];
    if (sscanf(command, "createCache %s", capacityStr) != 1){
        printf("Please provide a capacity value.\n");
        return;
    }
    if (!stringToInt(capacityStr, capacity)){
        printf("Invalid capacity. Please enter a valid integer.\n");
        return;
    }
    if (*capacity < 1 || *capacity > MAX_CAPACITY){
        printf("Please enter a capacity between 1-%d\n", MAX_CAPACITY);
        return;
    }
    cacheCapacity = *capacity;
    *cacheCreated = 1;
    createCache();
}

void put(int key, char* value){
    int index = hashMapSearch(key);
    if(index != -1){
        QueueNode* node = lruCache[index].queuePtr;
        free(node->value);
        node->value = stringCopy(value);
        removeQueueNode(node, 0);
        enqueue(node);
        return;
    }
    QueueNode* newNode = createQueueNode(key, value);
    if(!newNode){
        return;
    }
    if(cacheSize == cacheCapacity){
        int oldKey = rear->key;
        removeQueueNode(rear, 1);
        hashMapReplace(oldKey, key, newNode);
        enqueue(newNode);
        return;
    }
    hashMapInsert(key, newNode);
    enqueue(newNode);
    cacheSize++;
}

void handlePut(char* command, int cacheCreated){
    if (!cacheCreated){
        printf("Please create cache first.\n");
        return;
    }
    char keyStr[10];
    char *value = (char *)malloc(MAX_VALUE_SIZE * sizeof(char));
    if(!value){
        printf("Memory allocation failed.\n");
        return;
    }
    if (sscanf(command, "put %s %s", keyStr, value) != 2){
        printf("Invalid put command. Syntax: put <key> <value>.\n");
        free(value);
        return;
    }
    int key;
    if (!stringToInt(keyStr, &key)){
        printf("Invalid key. Please enter a valid integer.\n");
        free(value);
        return;
    }
    put(key, value);
}

void get(int key){
    int index = hashMapSearch(key);
    if(index == -1){
        printf("NULL\n");
        return;
    }
    QueueNode* queueNode = lruCache[index].queuePtr;
    printf("%s\n", queueNode->value);
    if(queueNode == front){
        return;
    }
    removeQueueNode(queueNode, 0);
    enqueue(queueNode);
}

void handleGet(char* command, int cacheCreated){
    if (!cacheCreated){
        printf("Please create cache first.\n");
        return;
    }
    char keyStr[10];
    if (sscanf(command, "get %s", keyStr) != 1){
        printf("Invalid get command. Syntax: get <key>.\n");
        return;
    }
    int key;
    if (!stringToInt(keyStr, &key)){
        printf("Invalid key. Please enter a valid integer.\n");
        return;
    }
    get(key);
}

void freeCache(){
    QueueNode* temp = front;
    while(temp){
        QueueNode* next = temp->next;
        free(temp->value);
        free(temp);
        temp = next;
    }

    if(lruCache != NULL){
        free(lruCache);
        lruCache = NULL;
    }
    front = NULL;
    rear = NULL;
}

void intializeLRUCache(){
    int cacheCreated = 0;
    int capacity = 0;
    char* command = (char*)malloc(MAX_COMMAND_SIZE * sizeof(char));
    if(!command){
        printf("Memory allocation failed.\n");
        return;
    }
    while(1){
        fgets(command, MAX_COMMAND_SIZE, stdin);
        if(command[strlen(command) - 1] == '\n'){
            command[strlen(command) - 1] = '\0';
        }
        char function[20];
        sscanf(command, "%s", function);

        if(stringCompare(function, "createCache") == 0){
            handleCreateCache(command, &cacheCreated, &capacity);
        }
        else if(stringCompare(function, "put") == 0){
            handlePut(command, cacheCreated);
        } 
        else if(stringCompare(function, "get") == 0){
            handleGet(command, cacheCreated);
        } 
        else if(stringCompare(function, "exit") == 0){
            free(command);
            freeCache();
            break;
        }
        else {
            printf("Invalid command.\n");
        }
    }
}

int main(){
    intializeLRUCache();
    return 0;
}
