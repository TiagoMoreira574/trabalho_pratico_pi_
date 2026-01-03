#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "unit.h"
#include "globals.h"

void initUnitCatalog(Unit catalog[NUM_UNIT_TYPES]) {
    catalog[0].type = UNIT_SWORDSMAN; strcpy(catalog[0].name, "Swordsman");
    catalog[0].attackPower = 15; catalog[0].health = 40; catalog[0].moveCost = 1;
    catalog[0].range = 1; catalog[0].cost = 20; catalog[0].symbol = 's'; catalog[0].owner = -1;

    catalog[1].type = UNIT_SPEARMAN; strcpy(catalog[1].name, "Spearman");
    catalog[1].attackPower = 18; catalog[1].health = 45; catalog[1].moveCost = 1;
    catalog[1].range = 1; catalog[1].cost = 25; catalog[1].symbol = 'p'; catalog[1].owner = -1;

    catalog[2].type = UNIT_ARCHER; strcpy(catalog[2].name, "Archer");
    catalog[2].attackPower = 12; catalog[2].health = 35; catalog[2].moveCost = 1;
    catalog[2].range = 2; catalog[2].cost = 30; catalog[2].symbol = 'a'; catalog[2].owner = -1;

    catalog[3].type = UNIT_KNIGHT; strcpy(catalog[3].name, "Knight");
    catalog[3].attackPower = 20; catalog[3].health = 70; catalog[3].moveCost = 2;
    catalog[3].range = 1; catalog[3].cost = 40; catalog[3].symbol = 'k'; catalog[3].owner = -1;

    catalog[4].type = UNIT_CATAPULT; strcpy(catalog[4].name, "Catapult");
    catalog[4].attackPower = 35; catalog[4].health = 50; catalog[4].moveCost = 3;
    catalog[4].range = 3; catalog[4].cost = 60; catalog[4].symbol = 't'; catalog[4].owner = -1;
}

void initUnitRequirements(UnitRequirement requirements[NUM_UNIT_TYPES]) {
    requirements[0].requiredBuilding = BUILDING_BARRACKS;
    strcpy(requirements[0].buildingName, "Barracks");

    requirements[1].requiredBuilding = BUILDING_BARRACKS;
    strcpy(requirements[1].buildingName, "Barracks");

    requirements[2].requiredBuilding = BUILDING_ARCHERY_RANGE;
    strcpy(requirements[2].buildingName, "Archery Range");

    requirements[3].requiredBuilding = BUILDING_STABLE;
    strcpy(requirements[3].buildingName, "Stable");

    requirements[4].requiredBuilding = BUILDING_SIEGE_WORKSHOP;
    strcpy(requirements[4].buildingName, "Siege Workshop");
}

void displayUnitCatalog(Unit catalog[NUM_UNIT_TYPES], UnitRequirement requirements[NUM_UNIT_TYPES]) {
    int i;
    printf("\n======================================================================================\n");
    printf("                              UNIT CATALOG                                            \n");
    printf("======================================================================================\n");
    printf(" Type | Name       | Req. Building   | Cost | Health | Attack | Range | Move | Symbol\n");
    printf("--------------------------------------------------------------------------------------\n");
    for (i = 0; i < NUM_UNIT_TYPES; i++) {
        printf("  %d   | %-10s | %-15s | %3d  |  %3d   |   %2d   |   %d   |   %d  |   %c\n",
            i, catalog[i].name, requirements[i].buildingName,
            catalog[i].cost, catalog[i].health, catalog[i].attackPower,
            catalog[i].range, catalog[i].moveCost, catalog[i].symbol);
    }
    printf("======================================================================================\n");
    printf("  -1  | Cancel                                                                        \n");
    printf("======================================================================================\n");
}

int placeUnit(char board[ROWS][COLS], Unit* u, int row, int col) {
    if (row < 0 || row >= ROWS || col < 0 || col >= COLS) {
        printf("Invalid placement: position outside the board.\n");
        return 0;
    }
    if (board[row][col] != EMPTY_CELL) {
        printf("Invalid placement: cell already occupied.\n");
        return 0;
    }
    u->row = row;
    u->col = col;
    board[row][col] = u->symbol;
    return 1;
}

UnitNode* addUnitToList(UnitNode** head, Unit* unit) {
    UnitNode* newNode = (UnitNode*)malloc(sizeof(UnitNode));
    UnitNode* current;
    if (newNode == NULL) {
        printf("Error: Failed to allocate memory for unit.\n");
        return NULL;
    }
    newNode->unit = *unit;
    newNode->next = NULL;
    if (*head == NULL) {
        *head = newNode;
    }
    else {
        current = *head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = newNode;
    }
    return newNode;
}

int removeUnitFromList(char board[ROWS][COLS], UnitNode** head, int row, int col) {
    UnitNode* current = *head;
    UnitNode* prev = NULL;
    while (current != NULL) {
        if (current->unit.row == row && current->unit.col == col) {
            board[row][col] = EMPTY_CELL;
            if (prev == NULL) {
                *head = current->next;
            }
            else {
                prev->next = current->next;
            }
            free(current);
            return 1;
        }
        prev = current;
        current = current->next;
    }
    return 0;
}

UnitNode* findUnitInList(UnitNode* head, int row, int col) {
    UnitNode* current = head;
    while (current != NULL) {
        if (current->unit.row == row && current->unit.col == col) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

int countUnitsInList(UnitNode* head) {
    int count = 0;
    UnitNode* current = head;
    while (current != NULL) {
        count++;
        current = current->next;
    }
    return count;
}

void freeUnitList(UnitNode** head) {
    UnitNode* current = *head;
    UnitNode* next;
    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }
    *head = NULL;
}

void displayUnitList(UnitNode* head, int playerNum) {
    UnitNode* current = head;
    int index = 0;
    char line[80];

    printf("\n    +--------------------------------------------------------------------+\n");
    sprintf(line, "Player %d's Units", playerNum + 1);
    printf("    |%*s%s%*s|\n", (68 - (int)strlen(line)) / 2, "", line, (68 - (int)strlen(line) + 1) / 2, "");
    printf("    +--------------------------------------------------------------------+\n");
    printf("    |  #  |    Name    |  HP  | Position |  ATK  | Range | Move Cost    |\n");
    printf("    +--------------------------------------------------------------------+\n");

    if (current == NULL) {
        printf("    |                     No units available                            |\n");
    }
    else {
        while (current != NULL) {
            printf("    |  %d  | %-10s |  %2d  |  (%2d,%2d) |  %2d   |   %d   |     %d        |\n",
                index, current->unit.name, current->unit.health,
                current->unit.row, current->unit.col,
                current->unit.attackPower, current->unit.range, current->unit.moveCost);
            index++;
            current = current->next;
        }
    }
    printf("    +--------------------------------------------------------------------+\n");
}

int findNearestFreeCell(char board[ROWS][COLS], int targetRow, int targetCol, int* foundRow, int* foundCol) {
    int distance, dr, dc, newRow, newCol;
    for (distance = 1; distance < ROWS + COLS; distance++) {
        for (dr = -distance; dr <= distance; dr++) {
            for (dc = -distance; dc <= distance; dc++) {
                if (abs(dr) + abs(dc) != distance) continue;
                newRow = targetRow + dr;
                newCol = targetCol + dc;
                if (newRow >= 0 && newRow < ROWS && newCol >= 0 && newCol < COLS) {
                    if (board[newRow][newCol] == EMPTY_CELL) {
                        *foundRow = newRow;
                        *foundCol = newCol;
                        return 1;
                    }
                }
            }
        }
    }
    return 0;
}

int moveUnit(Unit *u, int destRow, int destCol) {
    int distance, movementCost, * playerCoins, finalRow, finalCol;

    if (u->owner == UNIT_OWNER_PLAYER1) {
        playerCoins = &g_player1.coins;
    }
    else if (u->owner == UNIT_OWNER_PLAYER2) {
        playerCoins = &g_player2.coins;
    }
    else {
        printf("Error: Unit has no valid owner.\n");
        return 0;
    }

    if (destRow < 0 || destRow >= ROWS || destCol < 0 || destCol >= COLS) {
        printf("Invalid destination: position outside the board.\n");
        return 0;
    }

    distance = abs(destRow - u->row) + abs(destCol - u->col);
    if (distance == 0) {
        printf("Unit is already at that position.\n");
        return 0;
    }

    movementCost = distance * u->moveCost;
    if (*playerCoins < movementCost) {
        printf("Not enough coins to move this unit.\n");
        return 0;
    }

    finalRow = destRow;
    finalCol = destCol;

    if (g_board[destRow][destCol] != EMPTY_CELL) {
        printf("Destination occupied. Searching for nearest free cell...\n");
        if (!findNearestFreeCell(g_board, destRow, destCol, &finalRow, &finalCol)) {
            printf("No free cells available near the destination. Move cancelled.\n");
            return 0;
        }
        printf("Relocating to nearest free cell at (%d, %d) without extra cost.\n", finalRow, finalCol);
    }

    *playerCoins -= movementCost;
    g_board[u->row][u->col] = EMPTY_CELL;
    u->row = finalRow;
    u->col = finalCol;
    g_board[finalRow][finalCol] = u->symbol;

    printf("Unit '%s' moved to (%d, %d).\n", u->name, finalRow, finalCol);
    printf("Movement cost: %d coins. Remaining coins: %d\n", movementCost, *playerCoins);
    printBoard(g_board);
    return 1;
}

int attack(Unit* attacker, int targetRow, int targetCol) {
    int distance, attackerOwner, targetOwner;
    int isUnit = 0, isBuilding = 0;
    char* targetName = NULL;
    int* targetHealth = NULL;
    UnitNode* targetUnitNode = NULL;
    BuildingNode* targetBuildingNode = NULL;

    attackerOwner = attacker->owner;

    if (targetRow < 0 || targetRow >= ROWS || targetCol < 0 || targetCol >= COLS) {
        printf("Invalid target: position outside the board.\n");
        return 0;
    }

    if (g_board[targetRow][targetCol] == EMPTY_CELL) {
        printf("No target at the specified location.\n");
        return 0;
    }

    if (attackerOwner == 0) {
        targetUnitNode = findUnitInList(g_player2Units, targetRow, targetCol);
        if (targetUnitNode != NULL) {
            isUnit = 1;
            targetOwner = 1;
            targetName = targetUnitNode->unit.name;
            targetHealth = &targetUnitNode->unit.health;
        }
        else {
            if (findUnitInList(g_player1Units, targetRow, targetCol) != NULL) {
                printf("Cannot attack your own units!\n");
                return 0;
            }
        }
    }
    else {
        targetUnitNode = findUnitInList(g_player1Units, targetRow, targetCol);
        if (targetUnitNode != NULL) {
            isUnit = 1;
            targetOwner = 0;
            targetName = targetUnitNode->unit.name;
            targetHealth = &targetUnitNode->unit.health;
        }
        else {
            if (findUnitInList(g_player2Units, targetRow, targetCol) != NULL) {
                printf("Cannot attack your own units!\n");
                return 0;
            }
        }
    }

    if (!isUnit) {
        if (attackerOwner == 0) {
            targetBuildingNode = findBuildingInList(g_player2Buildings, targetRow, targetCol);
            if (targetBuildingNode != NULL) {
                isBuilding = 1;
                targetOwner = 1;
                targetName = targetBuildingNode->building.name;
                targetHealth = &targetBuildingNode->building.health;
            }
            else {
                if (findBuildingInList(g_player1Buildings, targetRow, targetCol) != NULL) {
                    printf("Cannot attack your own buildings!\n");
                    return 0;
                }
            }
        }
        else {
            targetBuildingNode = findBuildingInList(g_player1Buildings, targetRow, targetCol);
            if (targetBuildingNode != NULL) {
                isBuilding = 1;
                targetOwner = 0;
                targetName = targetBuildingNode->building.name;
                targetHealth = &targetBuildingNode->building.health;
            }
            else {
                if (findBuildingInList(g_player2Buildings, targetRow, targetCol) != NULL) {
                    printf("Cannot attack your own buildings!\n");
                    return 0;
                }
            }
        }
    }

    if (!isUnit && !isBuilding) {
        if (g_board[targetRow][targetCol] == 'B') {
            printf("Cannot attack bases directly in this version.\n");
            return 0;
        }
        printf("No target at the specified location.\n");
        return 0;
    }

    distance = abs(targetRow - attacker->row) + abs(targetCol - attacker->col);
    if (distance > attacker->range) {
        printf("Target out of range.\n");
        return 0;
    }

    printf("%s attacks %s!\n", attacker->name, targetName);
    printf("Attack power: %d, Target health before: %d\n", attacker->attackPower, *targetHealth);
    *targetHealth -= attacker->attackPower;
    printf("Target health after: %d\n", *targetHealth);

    if (*targetHealth <= 0) {
        printf("Target destroyed!\n");
        if (attackerOwner == 0) {
            addDestroyedEntity(&g_player1Kills, isUnit ? DESTROYED_UNIT : DESTROYED_BUILDING,
                targetName, targetOwner, targetRow, targetCol);
        }
        else {
            addDestroyedEntity(&g_player2Kills, isUnit ? DESTROYED_UNIT : DESTROYED_BUILDING,
                targetName, targetOwner, targetRow, targetCol);
        }
        if (isUnit) {
            if (targetOwner == 0) {
                removeUnitFromList(g_board, &g_player1Units, targetRow, targetCol);
                g_player1.numUnits--;
            }
            else {
                removeUnitFromList(g_board, &g_player2Units, targetRow, targetCol);
                g_player2.numUnits--;
            }
        }
        else if (isBuilding) {
            if (targetOwner == 0) {
                removeBuildingFromList(g_board, &g_player1Buildings, targetRow, targetCol);
                g_player1.numBuildings--;
            }
            else {
                removeBuildingFromList(g_board, &g_player2Buildings, targetRow, targetCol);
                g_player2.numBuildings--;
            }
        }
    }
    return 1;
}

DestroyedNode* addDestroyedEntity(DestroyedNode** head, int entityType, const char* name, int originalOwner, int row, int col) {
    DestroyedNode* newNode = (DestroyedNode*)malloc(sizeof(DestroyedNode));
    DestroyedNode* current;
    if (newNode == NULL) {
        printf("Error: Failed to allocate memory for destroyed entity.\n");
        return NULL;
    }
    newNode->entity.entityType = entityType;
    strncpy(newNode->entity.name, name, 19);
    newNode->entity.name[19] = '\0';
    newNode->entity.originalOwner = originalOwner;
    newNode->entity.row = row;
    newNode->entity.col = col;
    newNode->next = NULL;
    if (*head == NULL) {
        *head = newNode;
    }
    else {
        current = *head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = newNode;
    }
    return newNode;
}

int countDestroyedEntities(DestroyedNode* head) {
    int count = 0;
    DestroyedNode* current = head;
    while (current != NULL) {
        count++;
        current = current->next;
    }
    return count;
}

void displayDestroyedList(DestroyedNode* head, int playerNum) {
    DestroyedNode* current = head;
    int index = 0, unitsDestroyed = 0, buildingsDestroyed = 0;
    char line[80];

    sprintf(line, "Kills by Player %d", playerNum + 1);
    printf("    |%*s%s%*s|\n", (68 - (int)strlen(line)) / 2, "", line, (68 - (int)strlen(line) + 1) / 2, "");
    printf("    +--------------------------------------------------------------------+\n");

    if (current == NULL) {
        printf("    |                  No enemies defeated yet                          |\n");
    }
    else {
        while (current != NULL) {
            sprintf(line, "[%d] %s %s (was Player %d's) at (%d,%d)",
                index, current->entity.entityType == DESTROYED_UNIT ? "Unit:" : "Building:",
                current->entity.name, current->entity.originalOwner + 1,
                current->entity.row, current->entity.col);
            printf("    |  %-64s|\n", line);
            if (current->entity.entityType == DESTROYED_UNIT) unitsDestroyed++;
            else buildingsDestroyed++;
            index++;
            current = current->next;
        }
    }
    printf("    +--------------------------------------------------------------------+\n");
    sprintf(line, "Total: %d units, %d buildings destroyed", unitsDestroyed, buildingsDestroyed);
    printf("    |%*s%s%*s|\n", (68 - (int)strlen(line)) / 2, "", line, (68 - (int)strlen(line) + 1) / 2, "");
    printf("    +--------------------------------------------------------------------+\n");
}

void freeDestroyedList(DestroyedNode** head) {
    DestroyedNode* current = *head;
    DestroyedNode* next;
    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }
    *head = NULL;
}