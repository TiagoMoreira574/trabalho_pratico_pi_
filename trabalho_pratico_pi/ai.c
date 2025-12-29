#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include "ai.h"
#include "menu.h"
#include "globals.h"

int selectGameMode(void) {
    int choice;

    clearScreen();

    printBoxTop();
    printBoxEmpty();
    printBoxCenter("S E L E C T   G A M E   M O D E");
    printBoxEmpty();
    printBoxSeparator();
    printBoxEmpty();
    printf("    |                       [1]  Human vs Human                          |\n");
    printBoxCenter("Two players take turns on the same device");
    printBoxEmpty();
    printf("    |                       [2]  Human vs AI                             |\n");
    printBoxCenter("Challenge the computer opponent");
    printBoxEmpty();
    printBoxBottom();

    printf("\n                       Enter your choice: ");

    if (scanf("%d", &choice) != 1 || (choice != 1 && choice != 2)) {
        clearInputBuffer();
        return GAME_MODE_PVP;
    }
    clearInputBuffer();
    return choice;
}

int aiBuildMine(char board[ROWS][COLS], Player* aiPlayer, Building buildingCatalog[NUM_BUILDING_TYPES], BuildingNode** aiBuildings) {
    int row, col;
    Building newBuilding;

    if (aiPlayer->coins < buildingCatalog[BUILDING_MINE].cost) return 0;

    for (row = aiPlayer->baseRow - 2; row >= 0; row--) {
        for (col = aiPlayer->baseCol - 2; col >= 0; col--) {
            if (canPlaceBuilding(board, row, col, buildingCatalog[BUILDING_MINE].width, buildingCatalog[BUILDING_MINE].height)) {
                newBuilding = buildingCatalog[BUILDING_MINE];
                newBuilding.owner = 1;
                placeBuilding(board, &newBuilding, row, col);
                addBuildingToList(aiBuildings, &newBuilding);
                aiPlayer->numBuildings++;
                aiPlayer->coins -= buildingCatalog[BUILDING_MINE].cost;
                printf("      > AI builds Mine at (%d, %d)\n", row, col);
                return 1;
            }
        }
    }
    return 0;
}

int aiBuildBarracks(char board[ROWS][COLS], Player* aiPlayer, Building buildingCatalog[NUM_BUILDING_TYPES], BuildingNode** aiBuildings) {
    int row, col;
    Building newBuilding;

    if (aiPlayer->coins < buildingCatalog[BUILDING_BARRACKS].cost) return 0;
    if (hasBuildingInList(*aiBuildings, BUILDING_BARRACKS)) return 0;

    for (row = aiPlayer->baseRow - 3; row >= 0; row--) {
        for (col = aiPlayer->baseCol - 3; col >= 0; col--) {
            if (canPlaceBuilding(board, row, col, buildingCatalog[BUILDING_BARRACKS].width, buildingCatalog[BUILDING_BARRACKS].height)) {
                newBuilding = buildingCatalog[BUILDING_BARRACKS];
                newBuilding.owner = 1;
                placeBuilding(board, &newBuilding, row, col);
                addBuildingToList(aiBuildings, &newBuilding);
                aiPlayer->numBuildings++;
                aiPlayer->coins -= buildingCatalog[BUILDING_BARRACKS].cost;
                printf("      > AI builds Barracks at (%d, %d)\n", row, col);
                return 1;
            }
        }
    }
    return 0;
}

int aiRecruitUnit(char board[ROWS][COLS], Player* aiPlayer, Unit unitCatalog[NUM_UNIT_TYPES], UnitRequirement unitRequirements[NUM_UNIT_TYPES], UnitNode** aiUnits, BuildingNode* aiBuildings) {
    int unitType, row, col;
    Unit newUnit;

    for (unitType = 0; unitType < NUM_UNIT_TYPES; unitType++) {
        if (hasBuildingInList(aiBuildings, unitRequirements[unitType].requiredBuilding)) {
            if (aiPlayer->coins >= unitCatalog[unitType].cost) {
                for (row = aiPlayer->baseRow - 1; row >= 0; row--) {
                    for (col = aiPlayer->baseCol - 1; col >= 0; col--) {
                        if (board[row][col] == EMPTY_CELL) {
                            newUnit = unitCatalog[unitType];
                            newUnit.owner = UNIT_OWNER_PLAYER2;
                            if (placeUnit(board, &newUnit, row, col)) {
                                addUnitToList(aiUnits, &newUnit);
                                aiPlayer->numUnits++;
                                aiPlayer->coins -= unitCatalog[unitType].cost;
                                printf("      > AI recruits %s at (%d, %d)\n", newUnit.name, row, col);
                                return 1;
                            }
                        }
                    }
                }
            }
        }
    }
    return 0;
}

int aiAttack(int aiPlayerNum) {
    int distance, attacked = 0;
    UnitNode* aiCurrent, * humanCurrent, * aiUnits, * humanUnits;
    BuildingNode* humanBuildings, * buildingCurrent;

    if (aiPlayerNum == 0) {
        aiUnits = g_player1Units;
        humanUnits = g_player2Units;
        humanBuildings = g_player2Buildings;
    }
    else {
        aiUnits = g_player2Units;
        humanUnits = g_player1Units;
        humanBuildings = g_player1Buildings;
    }

    aiCurrent = aiUnits;
    while (aiCurrent != NULL) {
        humanCurrent = humanUnits;
        while (humanCurrent != NULL) {
            distance = abs(aiCurrent->unit.row - humanCurrent->unit.row) +
                abs(aiCurrent->unit.col - humanCurrent->unit.col);
            if (distance <= aiCurrent->unit.range) {
                printf("      > AI %s attacks enemy %s\n",
                    aiCurrent->unit.name, humanCurrent->unit.name);
                attack(&aiCurrent->unit, humanCurrent->unit.row, humanCurrent->unit.col);
                attacked = 1;
                break;
            }
            humanCurrent = humanCurrent->next;
        }
        if (!attacked) {
            buildingCurrent = humanBuildings;
            while (buildingCurrent != NULL) {
                distance = abs(aiCurrent->unit.row - buildingCurrent->building.row) +
                    abs(aiCurrent->unit.col - buildingCurrent->building.col);
                if (distance <= aiCurrent->unit.range) {
                    printf("      > AI %s attacks enemy %s\n",
                        aiCurrent->unit.name, buildingCurrent->building.name);
                    attack(&aiCurrent->unit, buildingCurrent->building.row, buildingCurrent->building.col);
                    attacked = 1;
                    break;
                }
                buildingCurrent = buildingCurrent->next;
            }
        }
        aiCurrent = aiCurrent->next;
    }
    return attacked;
}

int aiMoveTowardsEnemy(char board[ROWS][COLS], Player* aiPlayer, UnitNode** aiUnits, int enemyBaseRow, int enemyBaseCol) {
    UnitNode* current = *aiUnits;
    int moved = 0, targetRow, targetCol;

    (void)aiPlayer;

    while (current != NULL) {
        targetRow = current->unit.row;
        targetCol = current->unit.col;

        if (current->unit.row > enemyBaseRow) targetRow--;
        else if (current->unit.row < enemyBaseRow) targetRow++;

        if (current->unit.col > enemyBaseCol) targetCol--;
        else if (current->unit.col < enemyBaseCol) targetCol++;

        if (targetRow != current->unit.row || targetCol != current->unit.col) {
            if (targetRow >= 0 && targetRow < ROWS && targetCol >= 0 && targetCol < COLS) {
                if (board[targetRow][targetCol] == EMPTY_CELL) {
                    printf("      > AI moves %s towards enemy base\n", current->unit.name);
                    board[current->unit.row][current->unit.col] = EMPTY_CELL;
                    current->unit.row = targetRow;
                    current->unit.col = targetCol;
                    board[targetRow][targetCol] = current->unit.symbol;
                    moved = 1;
                }
            }
        }
        current = current->next;
    }
    return moved;
}

void executeAITurn(char board[ROWS][COLS], Player* aiPlayer, Player* humanPlayer, int aiPlayerNum, Building buildingCatalog[NUM_BUILDING_TYPES], BuildingNode** aiBuildings, BuildingNode** humanBuildings, Unit unitCatalog[NUM_UNIT_TYPES], UnitRequirement unitRequirements[NUM_UNIT_TYPES], UnitNode** aiUnits, UnitNode** humanUnits) {
    int actionsTaken = 0;
    int enemyBaseRow = humanPlayer->baseRow;
    int enemyBaseCol = humanPlayer->baseCol;

    (void)humanBuildings;
    (void)humanUnits;

    printf("\n");
    printBoxTop();
    printBoxEmpty();
    printBoxCenter("A I   I S   T H I N K I N G . . .");
    printBoxEmpty();
    printBoxBottom();
    printf("\n");

    if (!hasBuildingInList(*aiBuildings, BUILDING_BARRACKS)) {
        if (aiBuildBarracks(board, aiPlayer, buildingCatalog, aiBuildings)) {
            actionsTaken++;
        }
    }

    if (aiRecruitUnit(board, aiPlayer, unitCatalog, unitRequirements, aiUnits, *aiBuildings)) {
        actionsTaken++;
    }

    if (aiAttack(aiPlayerNum)) {
        actionsTaken++;
    }

    if (aiMoveTowardsEnemy(board, aiPlayer, aiUnits, enemyBaseRow, enemyBaseCol)) {
        actionsTaken++;
    }

    if (aiPlayer->coins >= buildingCatalog[BUILDING_MINE].cost + 20) {
        if (countBuildingTypeInList(*aiBuildings, BUILDING_MINE) < 3) {
            if (aiBuildMine(board, aiPlayer, buildingCatalog, aiBuildings)) {
                actionsTaken++;
            }
        }
    }

    if (actionsTaken == 0) {
        printf("      > AI has no available actions\n");
    }

    printf("\n      AI turn complete. Coins: %d\n", aiPlayer->coins);
    printBoard(board);
    waitForEnter();
}