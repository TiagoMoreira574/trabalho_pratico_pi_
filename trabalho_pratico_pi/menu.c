#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "menu.h"
#include "globals.h"

#define BOX_WIDTH 70

void clearInputBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void waitForEnter(void) {
    printf("\n                    Press [ENTER] to continue...");
    (void)getchar();
}

void printBoxTop(void) {
    int i;
    printf("\n    +");
    for (i = 0; i < BOX_WIDTH - 2; i++) printf("=");
    printf("+\n");
}

void printBoxBottom(void) {
    int i;
    printf("    +");
    for (i = 0; i < BOX_WIDTH - 2; i++) printf("=");
    printf("+\n");
}

void printBoxSeparator(void) {
    int i;
    printf("    +");
    for (i = 0; i < BOX_WIDTH - 2; i++) printf("-");
    printf("+\n");
}

void printBoxEmpty(void) {
    printf("    |%*s|\n", BOX_WIDTH - 2, "");
}

void printBoxCenter(const char* text) {
    int len = strlen(text);
    int padding = (BOX_WIDTH - 2 - len) / 2;
    int extra = (BOX_WIDTH - 2 - len) % 2;
    printf("    |%*s%s%*s|\n", padding, "", text, padding + extra, "");
}

void printBoxLeft(const char* text) {
    int len = strlen(text);
    printf("    |  %s%*s|\n", text, BOX_WIDTH - 4 - len, "");
}

int displayMainMenu(void) {
    int choice;

    clearScreen();

    printBoxTop();
    printBoxEmpty();
    printf("    |        _____ _   _ _____   _     ___ _____ ____                    |\n");
    printf("    |       |_   _| | | | ____| | |   / _ \\_   _|  _ \\                   |\n");
    printf("    |         | | | |_| |  _|   | |  | | | || | | |_) |                  |\n");
    printf("    |         | | |  _  | |___  | |__| |_| || | |  _ <                   |\n");
    printf("    |         |_| |_| |_|_____| |_____\\___/ |_| |_| \\_\\                  |\n");
    printBoxEmpty();
    printBoxCenter("The Lord of the Rings");
    printBoxCenter("by British author J. R. R. Tolkien");
    printBoxEmpty();
    printBoxSeparator();
    printBoxEmpty();
    printBoxCenter("M A I N   M E N U");
    printBoxEmpty();
    printf("    |                       [1]  Start New Game                          |\n");
    printf("    |                       [2]  Load Saved Game                         |\n");
    printf("    |                       [3]  Exit Game                               |\n");
    printBoxEmpty();
    printBoxBottom();

    printf("\n                       Enter your choice: ");

    if (scanf("%d", &choice) != 1) {
        clearInputBuffer();
        return -1;
    }
    clearInputBuffer();
    return choice;
}

void startNewGame(Player* player1, Player* player2, char board[ROWS][COLS]) {
    char name1[50], name2[50];
    int faction1, faction2;

    clearScreen();

    printBoxTop();
    printBoxEmpty();
    printBoxCenter("N E W   G A M E");
    printBoxCenter("Human vs Human");
    printBoxEmpty();
    printBoxSeparator();
    printBoxEmpty();
    printBoxCenter("Enter the names of both players");
    printBoxEmpty();
    printBoxBottom();

    printf("\n      Player 1 Name: ");
    if (fgets(name1, sizeof(name1), stdin) != NULL) {
        name1[strcspn(name1, "\n")] = '\0';
        /* Replace spaces with underscores for save/load compatibility */
        for (int i = 0; name1[i]; i++) if (name1[i] == ' ') name1[i] = '_';
    }
    if (strlen(name1) == 0) strcpy(name1, "Player1");

    printf("      Player 2 Name: ");
    if (fgets(name2, sizeof(name2), stdin) != NULL) {
        name2[strcspn(name2, "\n")] = '\0';
        /* Replace spaces with underscores for save/load compatibility */
        for (int i = 0; name2[i]; i++) if (name2[i] == ' ') name2[i] = '_';
    }
    if (strlen(name2) == 0) strcpy(name2, "Player2");

    clearScreen();

    faction1 = 0;
    while (faction1 != 1 && faction1 != 2) {
        clearScreen();

        printBoxTop();
        printBoxEmpty();
        printBoxCenter("C H O O S E   F A C T I O N");
        printBoxEmpty();
        printBoxSeparator();
        printBoxEmpty();
        printf("    |                       [1]  Free Peoples of Middle-Earth            |\n");
        printBoxCenter("(Gondor, Rohan, Rivendell)");
        printBoxEmpty();
        printf("    |                       [2]  Forces of Mordor                        |\n");
        printBoxCenter("(Sauron's Dark Army)");
        printBoxEmpty();
        printBoxBottom();

        printf("\n      %s, choose your faction (1-2): ", name1);

        if (scanf("%d", &faction1) != 1) {
            clearInputBuffer();
            faction1 = 0;
        }
        else {
            clearInputBuffer();
            if (faction1 != 1 && faction1 != 2) {
                printf("\n      Invalid choice. Please select 1 or 2.\n");
                waitForEnter();
            }
        }
    }

    faction2 = (faction1 == 1) ? 2 : 1;

    initPlayer(player1, name1, faction1, 100);
    initPlayer(player2, name2, faction2, 100);

    player1->baseRow = 0;
    player1->baseCol = 0;
    player2->baseRow = ROWS - 3;
    player2->baseCol = COLS - 3;

    Building base1, base2;
    base1 = g_buildingCatalog[BUILDING_BASE];
    base1.owner = 0;
    placeBuilding(board, &base1, player1->baseRow, player1->baseCol);
    addBuildingToList(&g_player1Buildings, &base1);
    player1->numBuildings = 1;

    base2 = g_buildingCatalog[BUILDING_BASE];
    base2.owner = 1;
    placeBuilding(board, &base2, player2->baseRow, player2->baseCol);
    addBuildingToList(&g_player2Buildings, &base2);
    player2->numBuildings = 1;

    clearScreen();

    printBoxTop();
    printBoxEmpty();
    printBoxCenter("G A M E   S E T U P   C O M P L E T E");
    printBoxEmpty();
    printBoxSeparator();
    printBoxEmpty();

    char line[80];
    sprintf(line, "Player 1: %s", player1->name);
    printBoxCenter(line);
    sprintf(line, "Faction: %s", getFactionName(player1->faction));
    printBoxCenter(line);
    sprintf(line, "Base Position: (%d, %d)", player1->baseRow, player1->baseCol);
    printBoxCenter(line);
    printBoxEmpty();
    printBoxSeparator();
    printBoxEmpty();
    sprintf(line, "Player 2: %s", player2->name);
    printBoxCenter(line);
    sprintf(line, "Faction: %s", getFactionName(player2->faction));
    printBoxCenter(line);
    sprintf(line, "Base Position: (%d, %d)", player2->baseRow, player2->baseCol);
    printBoxCenter(line);
    printBoxEmpty();
    printBoxSeparator();
    printBoxEmpty();
    printBoxCenter("Starting Coins: 100 each");
    printBoxEmpty();
    printBoxBottom();

    waitForEnter();
}

void startNewGameVsAI(Player* player1, Player* player2, char board[ROWS][COLS]) {
    char name1[50];
    int faction1, faction2;

    clearScreen();

    printBoxTop();
    printBoxEmpty();
    printBoxCenter("N E W   G A M E");
    printBoxCenter("Human vs AI");
    printBoxEmpty();
    printBoxSeparator();
    printBoxEmpty();
    printBoxCenter("Enter your name, brave warrior");
    printBoxEmpty();
    printBoxBottom();

    printf("\n      Your Name: ");
    if (fgets(name1, sizeof(name1), stdin) != NULL) {
        name1[strcspn(name1, "\n")] = '\0';
        /* Replace spaces with underscores for save/load compatibility */
        for (int i = 0; name1[i]; i++) if (name1[i] == ' ') name1[i] = '_';
    }
    if (strlen(name1) == 0) strcpy(name1, "Hero");

    faction1 = 0;
    while (faction1 != 1 && faction1 != 2) {
        clearScreen();

        printBoxTop();
        printBoxEmpty();
        printBoxCenter("C H O O S E   F A C T I O N");
        printBoxEmpty();
        printBoxSeparator();
        printBoxEmpty();
        printf("    |                       [1]  Free Peoples of Middle-Earth            |\n");
        printBoxCenter("(Gondor, Rohan, Rivendell)");
        printBoxEmpty();
        printf("    |                       [2]  Forces of Mordor                        |\n");
        printBoxCenter("(Sauron's Dark Army)");
        printBoxEmpty();
        printBoxBottom();

        printf("\n      %s, choose your faction (1-2): ", name1);

        if (scanf("%d", &faction1) != 1) {
            clearInputBuffer();
            faction1 = 0;
        }
        else {
            clearInputBuffer();
            if (faction1 != 1 && faction1 != 2) {
                printf("\n      Invalid choice. Please select 1 or 2.\n");
                waitForEnter();
            }
        }
    }

    faction2 = (faction1 == 1) ? 2 : 1;

    initPlayer(player1, name1, faction1, 100);
    initPlayer(player2, "AI", faction2, 100);

    player1->baseRow = 0;
    player1->baseCol = 0;
    player2->baseRow = ROWS - 3;
    player2->baseCol = COLS - 3;

    Building base1, base2;
    base1 = g_buildingCatalog[BUILDING_BASE];
    base1.owner = 0;
    placeBuilding(board, &base1, player1->baseRow, player1->baseCol);
    addBuildingToList(&g_player1Buildings, &base1);
    player1->numBuildings = 1;

    base2 = g_buildingCatalog[BUILDING_BASE];
    base2.owner = 1;
    placeBuilding(board, &base2, player2->baseRow, player2->baseCol);
    addBuildingToList(&g_player2Buildings, &base2);
    player2->numBuildings = 1;

    clearScreen();

    printBoxTop();
    printBoxEmpty();
    printBoxCenter("G A M E   S E T U P   C O M P L E T E");
    printBoxEmpty();
    printBoxSeparator();
    printBoxEmpty();

    char line[80];
    sprintf(line, "You: %s", player1->name);
    printBoxCenter(line);
    sprintf(line, "Faction: %s", getFactionName(player1->faction));
    printBoxCenter(line);
    printBoxEmpty();
    printBoxSeparator();
    printBoxEmpty();
    sprintf(line, "Enemy: %s [AI]", player2->name);
    printBoxCenter(line);
    sprintf(line, "Faction: %s", getFactionName(player2->faction));
    printBoxCenter(line);
    printBoxEmpty();
    printBoxSeparator();
    printBoxEmpty();
    printBoxCenter("May the Valar guide your path!");
    printBoxEmpty();
    printBoxBottom();

    waitForEnter();
}

int displayGameMenu(Player* player) {
    int choice;
    char line[80];

    printBoxTop();
    printBoxEmpty();
    sprintf(line, "%s's Turn", player->name);
    printBoxCenter(line);
    printBoxEmpty();
    printBoxSeparator();

    sprintf(line, "Faction: %-12s                Coins: %d", getFactionName(player->faction), player->coins);
    printBoxCenter(line);

    printBoxSeparator();
    printBoxEmpty();
    printf("    |%-68s|\n", "       [1] Build Structure             [5] Save Game");
    printf("    |%-68s|\n", "       [2] Recruit Unit                [6] End Turn");
    printf("    |%-68s|\n", "       [3] Move Unit                   [7] Quit Game");
    printf("    |%-68s|\n", "       [4] Attack");
    printBoxEmpty();
    printBoxBottom();

    printf("\n                       Enter your choice: ");

    if (scanf("%d", &choice) != 1) {
        clearInputBuffer();
        return -1;
    }
    clearInputBuffer();
    return choice;
}

void handleBuildAction(char board[ROWS][COLS], Player* player, Building buildingCatalog[NUM_BUILDING_TYPES], BuildingNode** playerBuildings) {
    int buildingType, row, col, continueBuiding = 1;
    Building newBuilding;
    char input[20];

    while (continueBuiding) {
        clearScreen();
        printBoard(board);

        printBoxTop();
        printBoxEmpty();
        printBoxCenter("B U I L D I N G   C A T A L O G");
        printBoxEmpty();
        printBoxSeparator();
        printf("    |  Type  |      Name        | Size | Cost | Health | Symbol          |\n");
        printBoxSeparator();
        printf("    |  [0]   | Base             | 3x3  |   0  |  200   |   B             |\n");
        printf("    |  [1]   | Mine             | 2x2  |  30  |   60   |   M             |\n");
        printf("    |  [2]   | Barracks         | 3x2  |  50  |  100   |   R             |\n");
        printf("    |  [3]   | Archery Range    | 3x2  |  40  |   80   |   A             |\n");
        printf("    |  [4]   | Stable           | 3x2  |  60  |  120   |   S             |\n");
        printf("    |  [5]   | Siege Workshop   | 3x2  |  70  |  140   |   W             |\n");
        printBoxSeparator();
        printBoxCenter("Note: Base [0] is placed automatically at game start");
        printBoxSeparator();

        char line[80];
        sprintf(line, "Your Coins: %d", player->coins);
        printBoxCenter(line);
        printBoxEmpty();
        printBoxBottom();

        printf("\n      Select building type (1-5) or [c] to cancel: ");
        if (scanf("%s", input) != 1) {
            clearInputBuffer();
            continue;
        }
        clearInputBuffer();

        if (input[0] == 'c' || input[0] == 'C') {
            break;
        }

        buildingType = atoi(input);

        if (buildingType == 0) {
            printf("\n      Base cannot be constructed manually.\n");
            waitForEnter();
            continue;
        }

        if (buildingType < 1 || buildingType > 5) {
            printf("\n      Invalid building type.\n");
            waitForEnter();
            continue;
        }

        if (player->coins < buildingCatalog[buildingType].cost) {
            printf("\n      Not enough coins! Need %d, have %d.\n",
                buildingCatalog[buildingType].cost, player->coins);
            waitForEnter();
            continue;
        }

        printf("      Enter row (0-%d) or [c] to cancel: ", ROWS - 1);
        if (scanf("%s", input) != 1) {
            clearInputBuffer();
            continue;
        }
        clearInputBuffer();

        if (input[0] == 'c' || input[0] == 'C') continue;
        row = atoi(input);

        printf("      Enter column (0-%d): ", COLS - 1);
        if (scanf("%d", &col) != 1) {
            clearInputBuffer();
            continue;
        }
        clearInputBuffer();

        if (!canPlaceBuilding(board, row, col, buildingCatalog[buildingType].width, buildingCatalog[buildingType].height)) {
            printf("\n      Cannot place building here - cells occupied or out of bounds.\n");
            waitForEnter();
            continue;
        }

        newBuilding = buildingCatalog[buildingType];
        newBuilding.owner = (player == &g_player1) ? 0 : 1;
        placeBuilding(board, &newBuilding, row, col);
        addBuildingToList(playerBuildings, &newBuilding);
        player->numBuildings++;
        player->coins -= buildingCatalog[buildingType].cost;

        clearScreen();
        printf("\n      Building '%s' placed at (%d, %d)!\n", newBuilding.name, row, col);
        printf("      Remaining coins: %d\n", player->coins);
        printBoard(board);

        printf("\n      Build another? [1] Yes  [0] No: ");
        if (scanf("%d", &continueBuiding) != 1) {
            clearInputBuffer();
            continueBuiding = 0;
        }
        clearInputBuffer();
    }
}

void handleRecruitAction(char board[ROWS][COLS], Player* player, Unit unitCatalog[NUM_UNIT_TYPES], UnitRequirement unitRequirements[NUM_UNIT_TYPES], UnitNode** playerUnits, BuildingNode* playerBuildings) {
    int unitType, row, col, continueRecruiting = 1;
    Unit newUnit;
    char input[20];

    while (continueRecruiting) {
        clearScreen();
        printBoard(board);

        printBoxTop();
        printBoxEmpty();
        printBoxCenter("U N I T   C A T A L O G");
        printBoxEmpty();
        printBoxSeparator();
        printf("    |  Type  |   Name    | Cost |  HP | ATK | Range | Required Building  |\n");
        printBoxSeparator();
        printf("    |  [0]   | Swordsman |  20  |  40 |  15 |   1   | Barracks           |\n");
        printf("    |  [1]   | Spearman  |  25  |  45 |  18 |   1   | Barracks           |\n");
        printf("    |  [2]   | Archer    |  30  |  35 |  12 |   2   | Archery Range      |\n");
        printf("    |  [3]   | Knight    |  40  |  70 |  20 |   1   | Stable             |\n");
        printf("    |  [4]   | Catapult  |  60  |  50 |  35 |   3   | Siege Workshop     |\n");
        printBoxSeparator();

        char line[80];
        sprintf(line, "Your Coins: %d", player->coins);
        printBoxCenter(line);
        printBoxEmpty();
        printBoxBottom();

        printf("\n      Select unit type (0-%d) or [c] to cancel: ", NUM_UNIT_TYPES - 1);

        if (scanf("%s", input) != 1) {
            clearInputBuffer();
            continue;
        }
        clearInputBuffer();

        if (input[0] == 'c' || input[0] == 'C') break;

        unitType = atoi(input);

        if (unitType < 0 || unitType >= NUM_UNIT_TYPES) {
            printf("\n      Invalid unit type.\n");
            waitForEnter();
            continue;
        }

        if (!hasBuildingInList(playerBuildings, unitRequirements[unitType].requiredBuilding)) {
            printf("\n      Cannot recruit %s - you need a %s first.\n",
                unitCatalog[unitType].name, unitRequirements[unitType].buildingName);
            waitForEnter();
            continue;
        }

        if (player->coins < unitCatalog[unitType].cost) {
            printf("\n      Not enough coins! Need %d, have %d.\n",
                unitCatalog[unitType].cost, player->coins);
            waitForEnter();
            continue;
        }

        printf("      Enter row (0-%d) or [c] to cancel: ", ROWS - 1);
        if (scanf("%s", input) != 1) {
            clearInputBuffer();
            continue;
        }
        clearInputBuffer();

        if (input[0] == 'c' || input[0] == 'C') continue;
        row = atoi(input);

        printf("      Enter column (0-%d): ", COLS - 1);
        if (scanf("%d", &col) != 1) {
            clearInputBuffer();
            continue;
        }
        clearInputBuffer();

        newUnit = unitCatalog[unitType];
        newUnit.owner = (player == &g_player1) ? UNIT_OWNER_PLAYER1 : UNIT_OWNER_PLAYER2;

        if (placeUnit(board, &newUnit, row, col)) {
            addUnitToList(playerUnits, &newUnit);
            player->numUnits++;
            player->coins -= unitCatalog[unitType].cost;

            clearScreen();
            printf("\n      Unit '%s' recruited at (%d, %d)!\n", newUnit.name, row, col);
            printf("      Remaining coins: %d\n", player->coins);
            printBoard(board);
        }
        else {
            waitForEnter();
            continue;
        }

        printf("\n      Recruit another? [1] Yes  [0] No: ");
        if (scanf("%d", &continueRecruiting) != 1) {
            clearInputBuffer();
            continueRecruiting = 0;
        }
        clearInputBuffer();
    }
}

void handleMoveAction(char board[ROWS][COLS], Player* player, UnitNode** playerUnits) {
    int destRow, destCol, continueMoving = 1, movementSuccess, i, unitChoice;
    UnitNode* current, * selectedNode;
    int currentNumUnits;
    char input[20];

    (void)board;
    (void)player;

    while (continueMoving) {
        clearScreen();
        printBoard(g_board);

        currentNumUnits = countUnitsInList(*playerUnits);
        if (currentNumUnits == 0) {
            printBoxTop();
            printBoxEmpty();
            printBoxCenter("No units available to move");
            printBoxEmpty();
            printBoxBottom();
            waitForEnter();
            break;
        }

        printBoxTop();
        printBoxEmpty();
        printBoxCenter("M O V E   U N I T");
        printBoxEmpty();
        printBoxSeparator();
        printBoxCenter("Select a unit to move:");
        printBoxEmpty();

        char line[80];
        current = *playerUnits;
        i = 0;
        while (current != NULL) {
            sprintf(line, "[%d] %s at (%d,%d) - HP: %d, Move Cost: %d/cell",
                i, current->unit.name, current->unit.row, current->unit.col,
                current->unit.health, current->unit.moveCost);
            printBoxCenter(line);
            current = current->next;
            i++;
        }

        printBoxEmpty();
        printBoxBottom();

        printf("\n      Select unit (0-%d) or [c] to cancel: ", currentNumUnits - 1);

        if (scanf("%s", input) != 1) {
            clearInputBuffer();
            continue;
        }
        clearInputBuffer();

        if (input[0] == 'c' || input[0] == 'C') break;

        unitChoice = atoi(input);

        if (unitChoice < 0 || unitChoice >= currentNumUnits) {
            printf("\n      Invalid selection.\n");
            waitForEnter();
            continue;
        }

        current = *playerUnits;
        for (i = 0; i < unitChoice && current != NULL; i++) {
            current = current->next;
        }
        selectedNode = current;

        if (selectedNode == NULL) {
            printf("\n      Error: Unit not found.\n");
            waitForEnter();
            continue;
        }

        printf("\n      Selected: %s at (%d,%d)\n", selectedNode->unit.name,
            selectedNode->unit.row, selectedNode->unit.col);

        movementSuccess = 0;
        while (!movementSuccess) {
            printf("      Destination row (0-%d) or [c] to cancel: ", ROWS - 1);
            if (scanf("%s", input) != 1) {
                clearInputBuffer();
                continue;
            }
            clearInputBuffer();

            if (input[0] == 'c' || input[0] == 'C') break;
            destRow = atoi(input);

            printf("      Destination column (0-%d): ", COLS - 1);
            if (scanf("%d", &destCol) != 1) {
                clearInputBuffer();
                continue;
            }
            clearInputBuffer();

            if (moveUnit(&selectedNode->unit, destRow, destCol)) {
                movementSuccess = 1;
            }
        }

        if (movementSuccess) {
            printf("\n      Move another unit? [1] Yes  [0] No: ");
            if (scanf("%d", &continueMoving) != 1) {
                clearInputBuffer();
                continueMoving = 0;
            }
            clearInputBuffer();
        }
    }
}

void handleAttackAction(char board[ROWS][COLS], int playerNum) {
    int targetRow, targetCol, continueAttacking = 1, attackSuccess, i, unitChoice;
    UnitNode* currentPlayerUnits, * current, * selectedNode;
    int currentNumUnits;
    char input[20];

    (void)board;

    if (playerNum == 0) {
        currentPlayerUnits = g_player1Units;
    }
    else {
        currentPlayerUnits = g_player2Units;
    }

    while (continueAttacking) {
        clearScreen();
        printBoard(g_board);

        currentNumUnits = countUnitsInList(currentPlayerUnits);
        if (currentNumUnits == 0) {
            printBoxTop();
            printBoxEmpty();
            printBoxCenter("No units available to attack with");
            printBoxEmpty();
            printBoxBottom();
            waitForEnter();
            break;
        }

        printBoxTop();
        printBoxEmpty();
        printBoxCenter("A T T A C K");
        printBoxEmpty();
        printBoxSeparator();
        printBoxCenter("Select an attacking unit:");
        printBoxEmpty();

        char line[80];
        current = currentPlayerUnits;
        i = 0;
        while (current != NULL) {
            sprintf(line, "[%d] %s at (%d,%d) - ATK: %d, Range: %d",
                i, current->unit.name, current->unit.row, current->unit.col,
                current->unit.attackPower, current->unit.range);
            printBoxCenter(line);
            current = current->next;
            i++;
        }

        printBoxEmpty();
        printBoxBottom();

        printf("\n      Select unit (0-%d) or [c] to cancel: ", currentNumUnits - 1);

        if (scanf("%s", input) != 1) {
            clearInputBuffer();
            continue;
        }
        clearInputBuffer();

        if (input[0] == 'c' || input[0] == 'C') break;

        unitChoice = atoi(input);

        if (unitChoice < 0 || unitChoice >= currentNumUnits) {
            printf("\n      Invalid selection.\n");
            waitForEnter();
            continue;
        }

        current = currentPlayerUnits;
        for (i = 0; i < unitChoice && current != NULL; i++) {
            current = current->next;
        }
        selectedNode = current;

        if (selectedNode == NULL) {
            printf("\n      Error: Unit not found.\n");
            waitForEnter();
            continue;
        }

        printf("\n      Selected: %s (ATK: %d, Range: %d)\n",
            selectedNode->unit.name, selectedNode->unit.attackPower, selectedNode->unit.range);

        attackSuccess = 0;
        while (!attackSuccess) {
            printf("      Target row (0-%d) or [c] to cancel: ", ROWS - 1);
            if (scanf("%s", input) != 1) {
                clearInputBuffer();
                continue;
            }
            clearInputBuffer();

            if (input[0] == 'c' || input[0] == 'C') break;
            targetRow = atoi(input);

            printf("      Target column (0-%d): ", COLS - 1);
            if (scanf("%d", &targetCol) != 1) {
                clearInputBuffer();
                continue;
            }
            clearInputBuffer();

            if (attack(&selectedNode->unit, targetRow, targetCol)) {
                printf("\n");
                printBoard(g_board);
                attackSuccess = 1;
            }
        }

        if (attackSuccess) {
            printf("\n      Attack with another unit? [1] Yes  [0] No: ");
            if (scanf("%d", &continueAttacking) != 1) {
                clearInputBuffer();
                continueAttacking = 0;
            }
            clearInputBuffer();
        }
    }
}

int countMinesInList(BuildingNode* head) {
    int mineCount = 0;
    BuildingNode* current = head;
    while (current != NULL) {
        if (current->building.type == BUILDING_MINE) {
            mineCount++;
        }
        current = current->next;
    }
    return mineCount;
}

void awardResourceIncome(Player* player, BuildingNode* playerBuildings, int playerNum) {
    int numMines = countMinesInList(playerBuildings);
    int income = 5 * numMines;
    if (income > 0) {
        player->coins += income;
        printf("\n      +%d coins from %d mine(s)\n", income, numMines);
    }
    (void)playerNum;
}

void endTurn(Player* player, int playerNum) {
    char line[80];

    if (player->coins < 0) {
        player->coins = 0;
    }

    clearScreen();

    printBoxTop();
    printBoxEmpty();
    printBoxCenter("E N D   O F   T U R N");
    printBoxEmpty();
    printBoxSeparator();
    printBoxEmpty();
    sprintf(line, "Player %d: %s", playerNum + 1, player->name);
    printBoxCenter(line);
    sprintf(line, "Coins remaining: %d", player->coins);
    printBoxCenter(line);
    printBoxEmpty();
    printBoxBottom();

    waitForEnter();
}