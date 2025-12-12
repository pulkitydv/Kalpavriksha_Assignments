#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_COMMAND_LENGTH 10
#define MAX_NAME_LENGTH 50
#define MAX_LINE_LENGTH 200
#define MAX_PROCESSES 1000

typedef enum
{
    READY,
    RUNNING,
    WAITING,
    TERMINATED
} ProcessState;

typedef enum
{
    EMPTY,
    OCCUPIED
} HashStatus;

typedef struct
{
    int pid;
    char *name;
    int burstTime;
    int remainingBurstTime;
    int ioStartTime;
    int ioDuration;
    int remainingIoTime;
    int executionTime;
    int turnAroundTime;
    int waitingTime;
    int completionTime;
    ProcessState state;
    int isKilled;
    int ioJustStarted;
} ProcessControlBlock;

typedef struct
{
    ProcessControlBlock *pcb;
    HashStatus status;
} HashEntry;

typedef struct QueueNode
{
    int pid;
    struct QueueNode *next;
} QueueNode;

typedef struct
{
    QueueNode *front;
    QueueNode *rear;
    int size;
} Queue;

typedef struct EventToKill
{
    int pid;
    int killTime;
    struct EventToKill *next;
} EventToKill;

HashEntry pcbHashMap[MAX_PROCESSES];

EventToKill *killEventHead = NULL;

Queue *readyQueue;
Queue *waitingQueue;
Queue *terminatedQueue;

int systemClock = 0;

int hashFunction(int pid)
{
    return pid % MAX_PROCESSES;
}

int stringLength(char *str)
{
    int length = 0;
    while (*str)
    {
        str++;
        length++;
    }
    return length;
}

char *stringCopy(char *src)
{
    char *dest = (char *)malloc(stringLength(src) + 1);
    if (!dest)
    {
        printf("Memory Allocation failed!\n");
        return NULL;
    }
    char *ptr = dest;
    while (*src)
    {
        *ptr = *src;
        ptr++;
        src++;
    }
    *ptr = '\0';
    return dest;
}

int stringCompare(char *str1, char *str2)
{
    while (*str1 && *str2)
    {
        if (*str1 < *str2)
        {
            return -1;
        }
        if (*str1 > *str2)
        {
            return 1;
        }
        str1++;
        str2++;
    }
    if (*str1)
    {
        return 1;
    }
    if (*str2)
    {
        return -1;
    }
    return 0;
}

Queue *initiateQueue()
{
    Queue *queue = (Queue *)malloc(sizeof(Queue));
    if (!queue)
    {
        printf("Memory Allocation failed!\n");
        return NULL;
    }
    queue->front = NULL;
    queue->rear = NULL;
    queue->size = 0;
    return queue;
}

QueueNode *createNode(int pid)
{
    QueueNode *newNode = (QueueNode *)malloc(sizeof(QueueNode));
    if (!newNode)
    {
        printf("Memory Allocation failed!\n");
        return NULL;
    }
    newNode->pid = pid;
    newNode->next = NULL;
    return newNode;
}

void enqueue(Queue *queue, int pid)
{
    QueueNode *newNode = createNode(pid);
    if (!newNode)
    {
        return;
    }

    if (queue->rear == NULL)
    {
        queue->front = newNode;
        queue->rear = newNode;
    }
    else
    {
        queue->rear->next = newNode;
        queue->rear = newNode;
    }
    (queue->size)++;
}

int isQueueEmpty(Queue *queue)
{
    return queue->front == NULL;
}

int peekQueue(Queue *queue)
{
    if (queue->front == NULL)
    {
        return -1;
    }
    return queue->front->pid;
}

int dequeue(Queue *queue)
{
    if (isQueueEmpty(queue))
    {
        return -1;
    }
    QueueNode *temp = queue->front;
    int popped = temp->pid;
    queue->front = queue->front->next;

    if (queue->front == NULL)
    {
        queue->rear = NULL;
    }

    free(temp);
    (queue->size)--;
    return popped;
}

void initializePCB(int pid, char *name, int burstTime, int ioStart, int ioDuration)
{
    ProcessControlBlock *pcb = (ProcessControlBlock *)malloc(sizeof(ProcessControlBlock));
    pcb->pid = pid;
    pcb->name = stringCopy(name);
    pcb->burstTime = burstTime;
    pcb->remainingBurstTime = burstTime;
    pcb->ioStartTime = ioStart;
    pcb->ioDuration = ioDuration;
    pcb->remainingIoTime = ioDuration;
    pcb->executionTime = 0;
    pcb->turnAroundTime = 0;
    pcb->waitingTime = 0;
    pcb->completionTime = 0;
    pcb->state = READY;
    pcb->isKilled = 0;
    pcb->ioJustStarted = 0;

    int index = hashFunction(pid);
    while (pcbHashMap[index].status == OCCUPIED)
    {
        index = (index + 1) % MAX_PROCESSES;
    }
    pcbHashMap[index].pcb = pcb;
    pcbHashMap[index].status = OCCUPIED;
    enqueue(readyQueue, pid);
}

ProcessControlBlock *getPCB(int pid)
{
    int index = hashFunction(pid);
    int originalIndex = index;

    while (pcbHashMap[index].status != EMPTY)
    {
        if (pcbHashMap[index].status == OCCUPIED && pcbHashMap[index].pcb->pid == pid)
        {
            return pcbHashMap[index].pcb;
        }
        index = (index + 1) % MAX_PROCESSES;
        if (index == originalIndex)
        {
            break;
        }
    }
    return NULL;
}

EventToKill *createEvent(int pid, int killTime)
{
    EventToKill *newEvent = (EventToKill *)malloc(sizeof(EventToKill));
    if (!newEvent)
    {
        printf("Memory Allocation failed.\n");
        return NULL;
    }
    newEvent->pid = pid;
    newEvent->killTime = killTime;
    newEvent->next = NULL;
    return newEvent;
}

void addKillEvent(int pid, int killTime)
{
    EventToKill *newEvent = createEvent(pid, killTime);
    if (!newEvent)
    {
        return;
    }

    if (killEventHead == NULL || killEventHead->killTime > killTime)
    {
        newEvent->next = killEventHead;
        killEventHead = newEvent;
    }
    else
    {
        EventToKill *current = killEventHead;
        while (current->next != NULL && current->next->killTime <= killTime)
        {
            current = current->next;
        }
        newEvent->next = current->next;
        current->next = newEvent;
    }
}

void processKillEvents()
{
    while (killEventHead != NULL && killEventHead->killTime == systemClock)
    {
        int pidToKill = killEventHead->pid;
        ProcessControlBlock *pcb = getPCB(pidToKill);
        if (pcb != NULL && !pcb->isKilled && pcb->state != TERMINATED)
        {
            pcb->isKilled = 1;
            pcb->state = TERMINATED;
            pcb->completionTime = systemClock;
        }
        EventToKill *temp = killEventHead;
        killEventHead = killEventHead->next;
        free(temp);
    }
}

void updateWaitingProcesses()
{
    if (isQueueEmpty(waitingQueue))
    {
        return;
    }
    QueueNode *current = waitingQueue->front;
    QueueNode *prev = NULL;

    while (current != NULL)
    {
        ProcessControlBlock *pcb = getPCB(current->pid);
        QueueNode *nextNode = current->next;
        if (pcb != NULL)
        {
            if (pcb->ioJustStarted)
            {
                pcb->ioJustStarted = 0;
            }
            else
            {
                if (pcb->remainingIoTime > 0)
                {
                    pcb->remainingIoTime--;
                    if (pcb->remainingIoTime == 0)
                    {
                        if (!pcb->isKilled)
                        {
                            pcb->state = READY;
                            enqueue(readyQueue, pcb->pid);
                        }
                        if (prev == NULL)
                        {
                            waitingQueue->front = current->next;
                            if (waitingQueue->front == NULL)
                            {
                                waitingQueue->rear = NULL;
                            }
                        }
                        else
                        {
                            prev->next = current->next;
                            if (current->next == NULL)
                            {
                                waitingQueue->rear = prev;
                            }
                        }
                        free(current);
                        (waitingQueue->size)--;
                        current = nextNode;
                        continue;
                    }
                }
            }
        }
        prev = current;
        current = nextNode;
    }
}

void startScheduler()
{
    int currentRunningPid = -1;

    while (!isQueueEmpty(readyQueue) || !isQueueEmpty(waitingQueue) || currentRunningPid != -1)
    {
        processKillEvents();
        if (currentRunningPid != -1)
        {
            ProcessControlBlock *runningPCB = getPCB(currentRunningPid);
            if (runningPCB->isKilled)
            {
                enqueue(terminatedQueue, currentRunningPid);
                currentRunningPid = -1;
            }
        }

        while (currentRunningPid == -1 && !isQueueEmpty(readyQueue))
        {
            int candidate = dequeue(readyQueue);
            ProcessControlBlock *pcb = getPCB(candidate);
            if (pcb == NULL)
            {
                continue;
            }
            if (pcb->isKilled)
            {
                enqueue(terminatedQueue, candidate);
                continue;
            }
            currentRunningPid = candidate;
            pcb->state = RUNNING;
            break;
        }

        if (currentRunningPid != -1)
        {
            ProcessControlBlock *pcb = getPCB(currentRunningPid);
            if (pcb != NULL && !pcb->isKilled && pcb->state == RUNNING)
            {
                sleep(1);
                (pcb->executionTime)++;
                (pcb->remainingBurstTime)--;
                if (pcb->ioStartTime > 0 && pcb->executionTime == pcb->ioStartTime && pcb->ioDuration > 0 && pcb->remainingBurstTime > 0)
                {
                    pcb->state = WAITING;
                    pcb->remainingIoTime = pcb->ioDuration;
                    pcb->ioJustStarted = 1;
                    enqueue(waitingQueue, currentRunningPid);
                    currentRunningPid = -1;
                }
                else if (pcb->remainingBurstTime == 0)
                {
                    pcb->state = TERMINATED;
                    pcb->completionTime = systemClock + 1;
                    enqueue(terminatedQueue, currentRunningPid);
                    currentRunningPid = -1;
                }
            }
            else
            {
                enqueue(terminatedQueue, currentRunningPid);
                currentRunningPid = -1;
            }
        }
        systemClock++;
        updateWaitingProcesses();
    }
}

void calculateValues()
{
    QueueNode *current = terminatedQueue->front;

    while (current != NULL)
    {
        ProcessControlBlock *pcb = getPCB(current->pid);
        pcb->turnAroundTime = pcb->completionTime;
        if (pcb->isKilled)
        {
            pcb->waitingTime = pcb->burstTime - pcb->executionTime;
        }
        else
        {
            pcb->waitingTime = pcb->turnAroundTime - pcb->burstTime;
        }
        current = current->next;
    }
}

void sortTerminatedQueueByPID()
{
    if (isQueueEmpty(terminatedQueue) || terminatedQueue->size <= 1)
    {
        return;
    }

    int *pidArray = (int*) malloc(terminatedQueue->size * sizeof(int));
    if (!pidArray)
    {
        printf("Memory allocation failed.\n");
        return;
    }

    QueueNode *current = terminatedQueue->front;
    int count = 0;
    while (current != NULL)
    {
        pidArray[count++] = current->pid;
        current = current->next;
    }

    for (int index = 0; index < count - 1; index++)
    {
        for (int process = 0; process < count - index - 1; process++)
        {
            if (pidArray[process] > pidArray[process + 1])
            {
                int temp = pidArray[process];
                pidArray[process] = pidArray[process + 1];
                pidArray[process + 1] = temp;
            }
        }
    }
    while (!isQueueEmpty(terminatedQueue))
    {
        dequeue(terminatedQueue);
    }

    for (int process = 0; process < count; process++)
    {
        enqueue(terminatedQueue, pidArray[process]);
    }

    free(pidArray);
}

void displayDetails()
{
    sortTerminatedQueueByPID();
    int hasAnyProcessKilled = 0;
    QueueNode *temp = terminatedQueue->front;
    while (temp != NULL)
    {
        ProcessControlBlock *pcb = getPCB(temp->pid);
        if (pcb->isKilled)
        {
            hasAnyProcessKilled = 1;
            break;
        }
        temp = temp->next;
    }

    printf("\n====================================================================================================\n");
    if (hasAnyProcessKilled)
    {
        printf("%-5s %-20s %-15s %-15s %-15s %-15s %-15s", "PID", "Name", "CPU", "IO", "Status", "Turnaround", "Waiting");
    }
    else
    {
        printf("%-5s %-20s %-15s %-15s %-15s %-15s", "PID", "Name", "CPU", "IO", "Turnaround", "Waiting");
    }
    printf("\n====================================================================================================\n");

    QueueNode *current = terminatedQueue->front;
    while (current != NULL)
    {
        ProcessControlBlock *pcb = getPCB(current->pid);

        if (hasAnyProcessKilled)
        {
            char status[20];
            if (pcb->isKilled)
            {
                sprintf(status, "KILLED at %d", pcb->completionTime);
                printf("%-5d %-20s %-15d %-15d %-15s %-15s %-15s\n", pcb->pid, pcb->name, pcb->burstTime, pcb->ioDuration, status, "-", "-");
            }
            else
            {
                sprintf(status, "OK");
                printf("%-5d %-20s %-15d %-15d %-15s %-15d %-15d\n", pcb->pid, pcb->name, pcb->burstTime, pcb->ioDuration, status, pcb->turnAroundTime, pcb->waitingTime);
            }
        }
        else
        {
            printf("%-5d %-20s %-15d %-15d %-15d %-15d\n", pcb->pid, pcb->name, pcb->burstTime, pcb->ioDuration, pcb->turnAroundTime, pcb->waitingTime);
        }

        current = current->next;
    }
    printf("\n====================================================================================================\n");
}

void readInput()
{
    char line[MAX_LINE_LENGTH];

    while (fgets(line, MAX_LINE_LENGTH, stdin) != NULL)
    {
        int length = stringLength(line);
        if (line[length - 1] == '\n')
        {
            line[length - 1] = '\0';
            length--;
        }

        if (length == 0)
        {
            break;
        }

        char command[MAX_COMMAND_LENGTH];
        sscanf(line, "%s", command);

        if (stringCompare(command, "KILL") == 0)
        {
            int pid, killTime;
            sscanf(line, "KILL %d %d", &pid, &killTime);
            addKillEvent(pid, killTime);
        }
        else
        {
            char name[MAX_NAME_LENGTH];
            int pid, burstTime;
            char ioStartStr[10], ioDurationStr[10];
            int ioStart = 0, ioDuration = 0;

            sscanf(line, "%s %d %d %s %s", name, &pid, &burstTime, ioStartStr, ioDurationStr);

            if (ioStartStr[0] != '-')
            {
                ioStart = atoi(ioStartStr);
            }

            if (ioDurationStr[0] != '-')
            {
                ioDuration = atoi(ioDurationStr);
            }

            initializePCB(pid, name, burstTime, ioStart, ioDuration);
        }
    }
}

void freeMemory()
{
    for (int i = 0; i < MAX_PROCESSES; i++)
    {
        if (pcbHashMap[i].status == OCCUPIED && pcbHashMap[i].pcb != NULL)
        {
            free(pcbHashMap[i].pcb->name);
            free(pcbHashMap[i].pcb);
        }
    }
    while (!isQueueEmpty(readyQueue))
    {
        dequeue(readyQueue);
    }
    while (!isQueueEmpty(waitingQueue))
    {
        dequeue(waitingQueue);
    }
    while (!isQueueEmpty(terminatedQueue))
    {
        dequeue(terminatedQueue);
    }

    free(readyQueue);
    free(waitingQueue);
    free(terminatedQueue);

    while (killEventHead != NULL)
    {
        EventToKill *temp = killEventHead;
        killEventHead = killEventHead->next;
        free(temp);
    }
}

void initiateScheduler()
{
    readInput();
    startScheduler();
    calculateValues();
    displayDetails();
    freeMemory();
}

int main()
{
    readyQueue = initiateQueue();
    waitingQueue = initiateQueue();
    terminatedQueue = initiateQueue();

    for (int i = 0; i < MAX_PROCESSES; i++)
    {
        pcbHashMap[i].pcb = NULL;
        pcbHashMap[i].status = EMPTY;
    }
    initiateScheduler();
}
