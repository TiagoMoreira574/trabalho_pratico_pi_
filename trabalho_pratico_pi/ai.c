#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "ai.h"
#include "menu.h"
#include "globals.h"

static int g_aiInitialized = 0;

static void initAIRandom(void) {
    if (!g_aiInitialized) {
        srand((unsigned int)time(NULL));
        g_aiInitialized = 1;
    }
}

int selectGameMode(void) {
    int choice = 0;

    while (choice != 1 && choice != 2) {
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

        printf("\n                       Enter your choice (1-2): ");

        if (scanf("%d", &choice) != 1) {
            clearInputBuffer();
            choice = 0;
        }
        else {
            clearInputBuffer();
            if (choice != 1 && choice != 2) {
                printf("\n      Invalid choice. Please select 1 or 2.\n");
                waitForEnter();
            }
        }
    }
    return choice;
}

static int aiBuildPhase(char board[ROWS][COLS], Player* aiPlayer,
    Building buildingCatalog[NUM_BUILDING_TYPES],
    BuildingNode** aiBuildings) {
    int row, col;
    Building newBuilding;
    int buildingType;
    int mineCount;
    int built = 0;

    if (!hasBuildingInList(*aiBuildings, BUILDING_BARRACKS)) {
        buildingType = BUILDING_BARRACKS;
        if (aiPlayer->coins >= buildingCatalog[buildingType].cost) {
            for (row = aiPlayer->baseRow - 3; row <= aiPlayer->baseRow + 4 && !built; row++) {
                for (col = aiPlayer->baseCol - 5; col <= aiPlayer->baseCol + 2 && !built; col++) {
                    if (row >= 0 && col >= 0 &&
                        row + buildingCatalog[buildingType].height <= ROWS &&
                        col + buildingCatalog[buildingType].width <= COLS) {
                        if (canPlaceBuilding(board, row, col,
                            buildingCatalog[buildingType].width,
                            buildingCatalog[buildingType].height)) {
                            newBuilding = buildingCatalog[buildingType];
                            newBuilding.owner = 1;
                            if (placeBuilding(board, &newBuilding, row, col)) {
                                addBuildingToList(aiBuildings, &newBuilding);
                                aiPlayer->numBuildings++;
                                aiPlayer->coins -= buildingCatalog[buildingType].cost;
                                printf("      > AI builds %s at (%d, %d) for %d coins\n",
                                    newBuilding.name, row, col, buildingCatalog[buildingType].cost);
                                built = 1;
                            }
                        }
                    }
                }
            }
        }
        if (!built) return 0;
    }

    mineCount = countBuildingTypeInList(*aiBuildings, BUILDING_MINE);
    while (mineCount < 2 && aiPlayer->coins >= buildingCatalog[BUILDING_MINE].cost) {
        buildingType = BUILDING_MINE;
        int minePlaced = 0;
        for (row = aiPlayer->baseRow - 4; row <= aiPlayer->baseRow + 5 && !minePlaced; row++) {
            for (col = aiPlayer->baseCol - 6; col <= aiPlayer->baseCol + 3 && !minePlaced; col++) {
                if (row >= 0 && col >= 0 &&
                    row + buildingCatalog[buildingType].height <= ROWS &&
                    col + buildingCatalog[buildingType].width <= COLS) {
                    if (canPlaceBuilding(board, row, col,
                        buildingCatalog[buildingType].width,
                        buildingCatalog[buildingType].height)) {
                        newBuilding = buildingCatalog[buildingType];
                        newBuilding.owner = 1;
                        if (placeBuilding(board, &newBuilding, row, col)) {
                            addBuildingToList(aiBuildings, &newBuilding);
                            aiPlayer->numBuildings++;
                            aiPlayer->coins -= buildingCatalog[buildingType].cost;
                            printf("      > AI builds %s at (%d, %d) for %d coins\n",
                                newBuilding.name, row, col, buildingCatalog[buildingType].cost);
                            built = 1;
                            minePlaced = 1;
                            mineCount++;
                        }
                    }
                }
            }
        }
        if (!minePlaced) break;
    }

    return built;
}

static int aiRecruitPhase(char board[ROWS][COLS], Player* aiPlayer,
    Unit unitCatalog[NUM_UNIT_TYPES],
    UnitRequirement unitRequirements[NUM_UNIT_TYPES],
    UnitNode** aiUnits, BuildingNode* aiBuildings) {
    int unitType, bestType = -1, bestCost = 9999;
    int row, col;
    Unit newUnit;

    for (unitType = 0; unitType < NUM_UNIT_TYPES; unitType++) {
        if (hasBuildingInList(aiBuildings, unitRequirements[unitType].requiredBuilding)) {
            if (aiPlayer->coins >= unitCatalog[unitType].cost) {
                if (unitCatalog[unitType].cost < bestCost) {
                    bestCost = unitCatalog[unitType].cost;
                    bestType = unitType;
                }
            }
        }
    }

    if (bestType >= 0) {
        for (row = aiPlayer->baseRow - 1; row <= aiPlayer->baseRow + 3; row++) {
            for (col = aiPlayer->baseCol - 1; col <= aiPlayer->baseCol + 3; col++) {
                if (row >= 0 && row < ROWS && col >= 0 && col < COLS) {
                    if (board[row][col] == EMPTY_CELL) {
                        newUnit = unitCatalog[bestType];
                        newUnit.owner = UNIT_OWNER_PLAYER2;
                        if (placeUnit(board, &newUnit, row, col)) {
                            addUnitToList(aiUnits, &newUnit);
                            aiPlayer->numUnits++;
                            aiPlayer->coins -= unitCatalog[bestType].cost;
                            printf("      > AI recruits %s at (%d, %d) for %d coins\n",
                                newUnit.name, row, col, unitCatalog[bestType].cost);
                            return 1;
                        }
                    }
                }
            }
        }
    }

    return 0;
}

static int aiAttackPhase(int aiPlayerNum) {
    UnitNode* aiUnits, * enemyUnits, * current, * target, * bestTarget;
    BuildingNode* enemyBuildings, * bTarget, * bestBTarget;
    int attacked = 0;
    int distance, bestDistance;
    int rowDiff, colDiff;

    if (aiPlayerNum == 0) {
        aiUnits = g_player1Units;
        enemyUnits = g_player2Units;
        enemyBuildings = g_player2Buildings;
    }
    else {
        aiUnits = g_player2Units;
        enemyUnits = g_player1Units;
        enemyBuildings = g_player1Buildings;
    }

    current = aiUnits;
    while (current != NULL) {
        bestTarget = NULL;
        bestBTarget = NULL;
        bestDistance = 9999;

        target = enemyUnits;
        while (target != NULL) {
            rowDiff = abs(current->unit.row - target->unit.row);
            colDiff = abs(current->unit.col - target->unit.col);
            distance = (rowDiff > colDiff) ? rowDiff : colDiff;

            if (distance <= current->unit.range && distance < bestDistance) {
                bestDistance = distance;
                bestTarget = target;
            }
            target = target->next;
        }

        if (bestTarget != NULL) {
            printf("      > AI %s at (%d,%d) attacks enemy %s at (%d,%d)\n",
                current->unit.name, current->unit.row, current->unit.col,
                bestTarget->unit.name, bestTarget->unit.row, bestTarget->unit.col);
            if (attack(&current->unit, bestTarget->unit.row, bestTarget->unit.col)) {
                attacked = 1;
            }
            current = current->next;
            continue;
        }

        bTarget = enemyBuildings;
        bestDistance = 9999;
        while (bTarget != NULL) {
            rowDiff = abs(current->unit.row - bTarget->building.row);
            colDiff = abs(current->unit.col - bTarget->building.col);
            distance = (rowDiff > colDiff) ? rowDiff : colDiff;

            if (distance <= current->unit.range && distance < bestDistance) {
                bestDistance = distance;
                bestBTarget = bTarget;
            }
            bTarget = bTarget->next;
        }

        if (bestBTarget != NULL) {
            printf("      > AI %s at (%d,%d) attacks enemy %s at (%d,%d)\n",
                current->unit.name, current->unit.row, current->unit.col,
                bestBTarget->building.name, bestBTarget->building.row, bestBTarget->building.col);
            if (attack(&current->unit, bestBTarget->building.row, bestBTarget->building.col)) {
                attacked = 1;
            }
        }

        current = current->next;
    }

    return attacked;
}

static int aiMovePhase(UnitNode* aiUnits, int enemyBaseRow, int enemyBaseCol,
    Player* aiPlayer) {
    UnitNode* current = aiUnits;
    int moved = 0;
    int newRow, newCol;
    int dRow, dCol;

    while (current != NULL) {
        if (aiPlayer->coins < current->unit.moveCost) {
            current = current->next;
            continue;
        }

        dRow = 0;
        dCol = 0;

        if (current->unit.row > enemyBaseRow) dRow = -1;
        else if (current->unit.row < enemyBaseRow) dRow = 1;

        if (current->unit.col > enemyBaseCol) dCol = -1;
        else if (current->unit.col < enemyBaseCol) dCol = 1;

        if (dRow == 0 && dCol == 0) {
            current = current->next;
            continue;
        }

        newRow = current->unit.row + dRow;
        newCol = current->unit.col + dCol;

        if (newRow >= 0 && newRow < ROWS && newCol >= 0 && newCol < COLS) {
            printf("      > AI moves %s from (%d,%d) towards enemy base...\n",
                current->unit.name, current->unit.row, current->unit.col);

            if (moveUnit(&current->unit, newRow, newCol)) {
                moved = 1;
            }
        }

        current = current->next;
    }

    return moved;
}

static int aiRandomPhase(UnitNode* aiUnits, Player* aiPlayer) {
    UnitNode* current = aiUnits;
    int moved = 0;
    int attempts, newRow, newCol;
    int dRow, dCol;

    initAIRandom();

    while (current != NULL) {
        if (aiPlayer->coins < current->unit.moveCost) {
            current = current->next;
            continue;
        }

        for (attempts = 0; attempts < 8; attempts++) {
            dRow = (rand() % 3) - 1;
            dCol = (rand() % 3) - 1;

            if (dRow == 0 && dCol == 0) continue;

            newRow = current->unit.row + dRow;
            newCol = current->unit.col + dCol;

            if (newRow < 0 || newRow >= ROWS || newCol < 0 || newCol >= COLS) continue;

            printf("      > AI randomly moves %s from (%d,%d)...\n",
                current->unit.name, current->unit.row, current->unit.col);

            if (moveUnit(&current->unit, newRow, newCol)) {
                moved = 1;
                break;
            }
        }

        current = current->next;
    }

    return moved;
}

void executeAITurn(char board[ROWS][COLS], Player* aiPlayer, Player* humanPlayer,
    int aiPlayerNum,
    Building buildingCatalog[NUM_BUILDING_TYPES],
    BuildingNode** aiBuildings, BuildingNode** humanBuildings,
    Unit unitCatalog[NUM_UNIT_TYPES],
    UnitRequirement unitRequirements[NUM_UNIT_TYPES],
    UnitNode** aiUnits, UnitNode** humanUnits) {

    int built = 0, recruited = 0, attacked = 0, moved = 0, randomMoved = 0;
    int enemyBaseRow = humanPlayer->baseRow;
    int enemyBaseCol = humanPlayer->baseCol;

    (void)humanBuildings;
    (void)humanUnits;

    if (!hasBuildingInList(*aiBuildings, BUILDING_BASE)) {
        Building base = buildingCatalog[BUILDING_BASE];
        base.owner = aiPlayerNum;
        base.row = aiPlayer->baseRow;
        base.col = aiPlayer->baseCol;
        addBuildingToList(aiBuildings, &base);
        aiPlayer->numBuildings++;
    }

    printf("\n");
    printBoxTop();
    printBoxEmpty();
    printBoxCenter("A I   T U R N");
    printBoxEmpty();
    printBoxBottom();
    printf("\n");

    awardResourceIncome(aiPlayer, *aiBuildings, aiPlayerNum);
    printf("      AI has %d coins and %d units\n\n", aiPlayer->coins, aiPlayer->numUnits);


    printf("      [Phase 1] Building...\n");
    built = aiBuildPhase(board, aiPlayer, buildingCatalog, aiBuildings);
    if (!built) {
        printf("      > No building needed or possible\n");
    }

    printf("\n      [Phase 2] Recruiting...\n");
    recruited = aiRecruitPhase(board, aiPlayer, unitCatalog, unitRequirements,
        aiUnits, *aiBuildings);
    if (!recruited) {
        printf("      > Cannot recruit (no coins/building/space)\n");
    }

    printf("\n      [Phase 3] Attacking...\n");
    attacked = aiAttackPhase(aiPlayerNum);
    if (!attacked) {
        printf("      > No targets in range\n");
    }

    printf("\n      [Phase 4] Moving towards enemy...\n");
    moved = aiMovePhase(*aiUnits, enemyBaseRow, enemyBaseCol, aiPlayer);
    if (!moved) {
        printf("      > Cannot move (no coins or blocked)\n");
    }

    if (!built && !recruited && !attacked && !moved) {
        printf("\n      [Phase 5] Random moves (fallback)...\n");
        randomMoved = aiRandomPhase(*aiUnits, aiPlayer);
        if (!randomMoved) {
            printf("      > No valid moves available\n");
        }
    }

    printf("\n      ========================================\n");
    printf("      AI Turn Complete:\n");
    printf("        - Built: %s\n", built ? "Yes" : "No");
    printf("        - Recruited: %s\n", recruited ? "Yes" : "No");
    printf("        - Attacked: %s\n", attacked ? "Yes" : "No");
    printf("        - Moved: %s\n", moved ? "Yes" : "No");
    if (!built && !recruited && !attacked && !moved) {
        printf("        - Random: %s\n", randomMoved ? "Yes" : "No");
    }
    printf("        - Coins: %d\n", aiPlayer->coins);
    printf("        - Units: %d\n", aiPlayer->numUnits);
    printf("      ========================================\n");

    printf("\n");
    printBoard(board);

    waitForEnter();
}