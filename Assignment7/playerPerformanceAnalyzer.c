#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "Players_data.h"

#define MAX_NAME_LENGTH 51
#define MAX_PLAYER_ID 1500
#define NUMBER_OF_TEAMS 10

typedef struct PlayerNode {
    int playerId;
    char* name;
    char* teamName;
    char* role;
    int totalRuns;
    float battingAverage;
    float strikeRate;
    int wickets;
    float economyRate;
    float performanceIndex;
    struct PlayerNode *next;
} PlayerNode;

typedef struct TeamNode {
    int teamID;
    char* name;
    int totalPlayers;
    int totalBatsmenAndAllRounders;
    float averageBattingStrikeRate;
    float avgStrikeRateBatterAllRounders;
    PlayerNode* batsmen;
    PlayerNode* bowlers;
    PlayerNode* allRounders;
} TeamNode;

TeamNode teamList[NUMBER_OF_TEAMS];

char* stringCopy(const char* source){
    char* destination = (char*)malloc(strlen(source) + 1);
    if(!destination){
        printf("Memory allocation failed\n");
        return NULL;
    }
    char* ptr = destination;
    while(*source){
        *ptr = *source;
        ptr++;
        source++;
    }
    *ptr = '\0';
    return destination;
}

void initializeTeam(){
    for(int team = 0; team < teamCount; team++){
        teamList[team].teamID = team+1;
        teamList[team].name = stringCopy(teams[team]);
        teamList[team].totalPlayers = 0;
        teamList[team].totalBatsmenAndAllRounders = 0;
        teamList[team].averageBattingStrikeRate = 0;
        teamList[team].avgStrikeRateBatterAllRounders = 0;
        teamList[team].batsmen = NULL;
        teamList[team].bowlers = NULL;
        teamList[team].allRounders = NULL;
    }
}

void insertSorted(PlayerNode** head, PlayerNode* newPlayer){
    if(*head == NULL || newPlayer->performanceIndex > (*head)->performanceIndex){
        newPlayer->next = *head;
        *head = newPlayer;
        return;
    }

    PlayerNode* current = *head;
    while(current->next != NULL && current->next->performanceIndex >= newPlayer->performanceIndex){
        current = current->next;
    }
    newPlayer->next = current->next;
    current->next = newPlayer;
}

int binarySearchTeam(const char* teamName){
    int left = 0;
    int right = teamCount - 1;
    while(left <= right){
        int mid = left + (right - left) / 2;
        int comparison = strcmp(teamList[mid].name, teamName);
        if(comparison == 0) {
            return mid;
        } else if(comparison < 0){
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    return -1;
}

void updateTeam(PlayerNode* newPlayer){
    int teamIndex = binarySearchTeam(newPlayer->teamName);
    if(teamIndex == -1){
        printf("No team found.\n");
        return;
    }
    if (strcmp(newPlayer->role, "Batsman") == 0)
    {
        insertSorted(&teamList[teamIndex].batsmen, newPlayer);
        teamList[teamIndex].avgStrikeRateBatterAllRounders = ((teamList[teamIndex].avgStrikeRateBatterAllRounders * teamList[teamIndex].totalBatsmenAndAllRounders) + newPlayer->strikeRate) / (teamList[teamIndex].totalBatsmenAndAllRounders + 1);
        teamList[teamIndex].totalBatsmenAndAllRounders++;
    }
    else if (strcmp(newPlayer->role, "All-rounder") == 0)
    {
        insertSorted(&teamList[teamIndex].allRounders, newPlayer);
        teamList[teamIndex].avgStrikeRateBatterAllRounders = ((teamList[teamIndex].avgStrikeRateBatterAllRounders * teamList[teamIndex].totalBatsmenAndAllRounders) + newPlayer->strikeRate) / (teamList[teamIndex].totalBatsmenAndAllRounders + 1);
        teamList[teamIndex].totalBatsmenAndAllRounders++;
    }
    else
    {
        insertSorted(&teamList[teamIndex].bowlers, newPlayer);
    }
    teamList[teamIndex].averageBattingStrikeRate = ((teamList[teamIndex].averageBattingStrikeRate * teamList[teamIndex].totalPlayers) + newPlayer->strikeRate) / (teamList[teamIndex].totalPlayers + 1);
    teamList[teamIndex].totalPlayers++;
}

float calculatePerformanceIndex(PlayerNode* newPlayer){
    float performanceIndex = 0;
    if (strcmp(newPlayer->role, "Batsman") == 0)
    {
        performanceIndex = (newPlayer->battingAverage * newPlayer->strikeRate) / 100.0f;
    }
    else if (strcmp(newPlayer->role, "Bowler") == 0)
    {
        performanceIndex = (newPlayer->wickets * 2.0f) + (100.0f - newPlayer->economyRate);
    }
    else if (strcmp(newPlayer->role, "All-rounder") == 0)
    {
        performanceIndex = ((newPlayer->battingAverage * newPlayer->strikeRate) / 100.0f) + (newPlayer->wickets * 2);
    }
    return performanceIndex;
}

void initializePlayers(){
    for(int playerNumber = 0; playerNumber < playerCount; playerNumber++){
        PlayerNode* newPlayer = (PlayerNode*)malloc(sizeof(PlayerNode));
        if(!newPlayer){
            printf("Memory allocation Failed!\n");
            return;
        }
        newPlayer->playerId = players[playerNumber].id;
        newPlayer->name = stringCopy(players[playerNumber].name);
        newPlayer->teamName = stringCopy(players[playerNumber].team);
        newPlayer->role = stringCopy(players[playerNumber].role);
        newPlayer->totalRuns = players[playerNumber].totalRuns;
        newPlayer->battingAverage = players[playerNumber].battingAverage;
        newPlayer->strikeRate = players[playerNumber].strikeRate;
        newPlayer->wickets = players[playerNumber].wickets;
        newPlayer->economyRate = players[playerNumber].economyRate;
        newPlayer->performanceIndex = calculatePerformanceIndex(newPlayer);
        newPlayer->next = NULL;
        updateTeam(newPlayer);
    }
}

int getValidRole(){
    int role;
    while(1){
        if(scanf("%d", &role) != 1){
            printf("Invalid Role. Please enter a valid role (1-3): ");
            while (getchar() != '\n');
        } 
        else {
            if(getchar() != '\n' || role < 1 || role > 3){
                printf("Invalid Role. Please enter a valid role (1-3): ");
                while (getchar() != '\n');
            } else {
                return role;
            }
        }
    } 
}

int getValidInteger(){
    int integer;
    while (1){
        if (scanf("%d", &integer) != 1)
        {
            printf("Please Enter the valid integer only: ");
            while (getchar() != '\n');
        }
        else 
        {
            if (getchar() != '\n')
            {
                printf("Please Enter the valid integer only: ");
                while (getchar() != '\n');
            }
            else
            {
                return integer;
            }
        }
    }
}

float getValidFloat(){
    float number;
    while (1)
    {
        if (scanf("%f", &number) != 1)
        {
            printf("Please Enter a valid number: ");
            while (getchar() != '\n');
        }
        else
        {
            if (getchar() != '\n')
            {
                printf("Please Enter a valid number: ");
                while (getchar() != '\n');
            }
            else
            {
                return number;
            }
        }
    }
}

int getValidTeamID(){
    while (1){
        int id = getValidInteger();
        if(id < 1 || id > 10){
            printf("Enter Team ID in range 1-10: ");
        } else {
            return id;
        }
    }
}

int playerIdExists(int id){
    for(int index = 0; index < teamCount; index++){
        PlayerNode* temp = teamList[index].batsmen;
        while(temp != NULL){
            if(temp->playerId == id) {
                return 1;
            }
            temp = temp->next;
        }
        temp = teamList[index].allRounders;
        while(temp != NULL){
            if(temp->playerId == id) {
                return 1;
            }
            temp = temp->next;
        }
        temp = teamList[index].bowlers;
        while(temp != NULL){
            if(temp->playerId == id) {
                return 1;
            }
            temp = temp->next;
        }
    }
    return 0;
}

int getValidPlayerID(){
    int id;
    while(1){
        id = getValidInteger();
        if(id < 1 || id > MAX_PLAYER_ID){
            printf("Enter ID in range 1-1000: ");
        } 
        else if(playerIdExists(id)){
            printf("ID already exists! Enter unique ID: ");
        }
        else{
            return id;
        }
    }
}

void addNewPlayer(){
    PlayerNode* newPlayer = (PlayerNode*)malloc(sizeof(PlayerNode));
    if(!newPlayer){
        printf("Memory allocation Failed!\n");
        return;
    }
    printf("Enter Team ID to add player: ");
    int teamId = getValidTeamID();
    newPlayer->teamName = stringCopy(teamList[teamId - 1].name);
    printf("Enter Player Details: \n");

    printf("Player ID: ");
    newPlayer->playerId = getValidPlayerID();

    printf("Name: ");
    char* name = (char*)malloc(MAX_NAME_LENGTH);
    fgets(name, MAX_NAME_LENGTH, stdin);
    if(*(name + strlen(name) - 1) == '\n'){
        *(name + strlen(name) - 1) = '\0';
    }
    newPlayer->name = stringCopy(name);

    printf("Role (1-Batsman, 2-Bowler, 3-All-Rounder): ");
    int role = getValidRole();
    if(role == 1) {
        newPlayer->role = stringCopy("Batsman");
    } else if(role == 2){
        newPlayer->role = stringCopy("Bowler");
    } else {
        newPlayer->role = stringCopy("All-rounder");
    }

    printf("Total Runs: ");
    newPlayer->totalRuns = getValidInteger();

    printf("Batting Average: ");
    newPlayer->battingAverage = getValidFloat();

    printf("Strike Rate: ");
    newPlayer->strikeRate = getValidFloat();
    
    printf("Wickets: ");
    newPlayer->wickets = getValidInteger();

    printf("Economy Rate: ");
    newPlayer->economyRate = getValidFloat();

    newPlayer->performanceIndex = calculatePerformanceIndex(newPlayer);
    newPlayer->next = NULL;
    updateTeam(newPlayer);

    printf("Player added successfully to Team %s!", newPlayer->teamName);
}

void playerDetails(PlayerNode* newPlayer){
    printf("%-5d %-20s %-15s %-8d %-8.2f %-8.2f %-8d %-8.2f %-12.2f\n", newPlayer->playerId, newPlayer->name, newPlayer->role, newPlayer->totalRuns, newPlayer->battingAverage, newPlayer->strikeRate, newPlayer->wickets, newPlayer->economyRate, newPlayer->performanceIndex);
}

void displayPlayersOfTeam(){
    printf("Enter Team ID: ");
    int teamID = getValidTeamID();
    printf("Players of Team %s:\n", teamList[teamID - 1].name);
    printf("\n ===============================================================================================\n");
    printf(" %-5s %-20s %-15s %-8s %-8s %-8s %-8s %-8s %-12s\n", "ID", "Name", "Role", "Runs", "Avg", "SR", "Wkts", "ER", "Perf. Index");
    printf("\n ===============================================================================================\n");
    PlayerNode* temp = teamList[teamID - 1].batsmen;
    while(temp != NULL){
        playerDetails(temp);
        temp = temp->next;
    }
    temp = teamList[teamID - 1].allRounders;
    while(temp != NULL){
        playerDetails(temp);
        temp = temp->next;
    }
    temp = teamList[teamID - 1].bowlers;
    while(temp != NULL){
        playerDetails(temp);
        temp = temp->next;
    }
    
    printf("\n ===============================================================================================\n");
    printf("Total Players: %d\n", teamList[teamID-1].totalPlayers);
    printf("Average Batting Strike Rate: %.2f\n", teamList[teamID - 1].averageBattingStrikeRate);
}

void displayTeamsBySR(){
    int teamIndices[teamCount];
    for(int team = 0; team < teamCount; team++){
        teamIndices[team] = team;
    }

    for (int index = 0; index < teamCount; index++)
    {
        for (int index2 = 0; index2 < teamCount - index - 1; index2++)
        {
            if (teamList[teamIndices[index2]].avgStrikeRateBatterAllRounders < teamList[teamIndices[index2 + 1]].avgStrikeRateBatterAllRounders)
            {
                int temp = teamIndices[index2];
                teamIndices[index2] = teamIndices[index2 + 1];
                teamIndices[index2 + 1] = temp;
            }
        }
    }
    printf("Teams sorted by Average Batting Strike Rate\n");
    printf("\n ===================================================================\n");
    printf(" %-3s %-15s %-5s %-14s\n", "ID", "Team Name", "Avg Bat SR", "Total Players");
    printf("\n ===================================================================\n");
    
    for (int i = 0; i < teamCount; i++){
        printf("%-3d %-15s %-10.2f %-2d\n", teamList[teamIndices[i]].teamID, teamList[teamIndices[i]].name, teamList[teamIndices[i]].avgStrikeRateBatterAllRounders, teamList[teamIndices[i]].totalPlayers);
    }
    printf("\n ===================================================================\n");
}

void displayTopKPlayers(){
    printf("Enter Team ID: ");
    int teamID = getValidTeamID();
    printf("Enter Role(1-Batsman, 2-Bowler, 3-All-rounder): ");
    int role = getValidRole();
    printf("Enter number of players: ");
    int k = getValidInteger();

    printf("Top %d %s of Team %s:\n", k, (role == 1? "Batsmen" : (role == 2 ? "Bowlers" : "All-rounders")) , teamList[teamID - 1].name);
    printf("\n ======================================================================================================\n");
    printf(" %-5s %-20s %-15s %-8s %-8s %-8s %-8s %-8s %-12s\n", "ID", "Name", "Role", "Runs", "Avg", "SR", "Wkts", "ER", "Perf. Index");
    printf("\n ======================================================================================================\n");
    PlayerNode* temp;
    if(role == 1){
        temp = teamList[teamID - 1].batsmen;
    }
    else if(role == 2){
        temp = teamList[teamID - 1].bowlers;
    }
    else{
        temp = teamList[teamID - 1].allRounders;
    }
    while(k-- && temp != NULL){
        playerDetails(temp);
        temp = temp->next;
    }
}

void displayAllPlayersByRole(){
    printf("Enter Role(1-Batsman, 2-Bowler, 3-All-rounder): ");
    int role = getValidRole();
    PlayerNode* teamPlayers[teamCount];
    for(int index = 0; index < teamCount; index++){
        if(role == 1){
            teamPlayers[index] = teamList[index].batsmen;
        } else if(role == 2){
            teamPlayers[index] = teamList[index].bowlers;            
        } else {
            teamPlayers[index] = teamList[index].allRounders;
        }
    }
    printf("\n =========================================================================================================================\n");
    printf("%-5s %-20s %-15s %-15s %-8s %-8s %-8s %-8s %-8s %-12s\n", "ID", "Name", "Team", "Role", "Runs", "Avg", "SR", "Wkts", "ER", "Perf. Index");
    printf("\n =========================================================================================================================\n");
    
    while(1){
        int bestIndex = -1;
        int bestPerformanceIndex = -1;

        for(int index = 0; index < teamCount; index++){
            if(teamPlayers[index] != NULL){
                if(teamPlayers[index]->performanceIndex > bestPerformanceIndex){
                    bestPerformanceIndex = teamPlayers[index]->performanceIndex;
                    bestIndex = index;
                }
            }
        }

        if(bestIndex == -1) {
            break;
        }
        PlayerNode* temp = teamPlayers[bestIndex];
        printf("%-5d %-20s %-15s %-15s %-8d %-8.2f %-8.2f %-8d %-8.2f %-12.2f\n", temp->playerId, temp->name, temp->teamName, temp->role, temp->totalRuns, temp->battingAverage, temp->strikeRate, temp->wickets, temp->economyRate, temp->performanceIndex);
        teamPlayers[bestIndex] = teamPlayers[bestIndex]->next;
    }
    printf("\n =========================================================================================================================\n");
}

void freeMemory(){
    for(int index = 0; index < teamCount; index++){
        PlayerNode* lists[3] = {
            teamList[index].batsmen,
            teamList[index].bowlers,
            teamList[index].allRounders
        };

        for(int role = 0; role < 3; role++){
            PlayerNode* current = lists[role];
            while(current != NULL){
                PlayerNode* temp = current;
                current = current->next;
                free(temp->name);
                free(temp->teamName);
                free(temp->role);
                free(temp);
            }
        }
        free(teamList[index].name);
    }
}

void displayMenu(){
    printf("\n ==============================================================================\n");
    printf(" ICC ODI Player Performance Analyzer");
    printf("\n ==============================================================================\n");
    printf("1. Add Player to Team\n");
    printf("2. Display Players of a Specific Team\n");
    printf("3. Display Teams by Average Batting Strike Rate\n");
    printf("4. Display Top K Players of a Specific Team by Role\n");
    printf("5. Display all Players of specific role Across All Teams by performance index\n");
    printf("6. Exit \n");
    printf("\n ==============================================================================\n");
    printf("Enter your choice: ");
}

void initiateAnalyzer(){
    int choice = 0;
    while(choice != 6){
        displayMenu();
        choice = getValidInteger();
        switch (choice)
        {
        case 1:
            addNewPlayer();
            break;
        case 2:
            displayPlayersOfTeam();
            break;
        case 3:
            displayTeamsBySR();
            break;
        case 4:
            displayTopKPlayers();
            break;
        case 5:
            displayAllPlayersByRole();
            break;
        case 6:
            freeMemory();
            return;
        default:
            printf("Invalid choice\n.");
        }
    }
}

int main(){
    initializeTeam();
    initializePlayers();
    initiateAnalyzer();
}
