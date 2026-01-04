#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "save.h"
#include "globals.h"
#include "menu.h"

#define SAVE_MARKER_START "LOTR_SAVE_START"
#define SAVE_MARKER_END "LOTR_SAVE_END"
#define BOARD_START "BOARD_START"
#define BOARD_END "BOARD_END"
#define P1_BUILDINGS_START "P1_BUILDINGS_START"
#define P1_BUILDINGS_END "P1_BUILDINGS_END"
#define P2_BUILDINGS_START "P2_BUILDINGS_START"
#define P2_BUILDINGS_END "P2_BUILDINGS_END"
#define P1_UNITS_START "P1_UNITS_START"
#define P1_UNITS_END "P1_UNITS_END"
#define P2_UNITS_START "P2_UNITS_START"
#define P2_UNITS_END "P2_UNITS_END"

#define SAVES_DIR "saves"
#define MAX_SAVES 20
#define MAX_FILENAME 100

static void ensureSavesDirectory(void) {
#ifdef _WIN32
    system("if not exist saves mkdir saves 2>nul");
#else
    system("mkdir -p saves 2>/dev/null");
#endif
}

static int listSaveFiles(char saveFiles[][MAX_FILENAME], int maxFiles) {
    FILE* fp;
    int count = 0;
    char line[256];

#ifdef _WIN32
    fp = _popen("dir /b saves\\*.txt 2>nul", "r");
#else
    fp = popen("ls saves/*.txt 2>/dev/null | xargs -n1 basename 2>/dev/null", "r");
#endif

    if (fp == NULL) {
        return 0;
    }

    while (fgets(line, sizeof(line), fp) != NULL && count < maxFiles) {
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
            len--;
        }
        if (len > 0 && line[len - 1] == '\r') {
            line[len - 1] = '\0';
            len--;
        }

        if (len > 0) {
            strncpy(saveFiles[count], line, MAX_FILENAME - 1);
            saveFiles[count][MAX_FILENAME - 1] = '\0';
            count++;
        }
    }

#ifdef _WIN32
    _pclose(fp);
#else
    pclose(fp);
#endif

    return count;
}

void saveGame(const char* filename) {
    FILE* file;
    char fullPath[256];
    int i, j;
    UnitNode* unitCurrent;
    BuildingNode* buildingCurrent;

    ensureSavesDirectory();

    snprintf(fullPath, sizeof(fullPath), "%s/%s", SAVES_DIR, filename);

    file = fopen(fullPath, "w");
    if (file == NULL) {
        printf("Error: Could not create save file '%s'.\n", fullPath);
        return;
    }

    fprintf(file, "%s\n", SAVE_MARKER_START);
    fprintf(file, "CURRENT_PLAYER %d\n", g_currentPlayerNum);
    fprintf(file, "GAME_MODE %d\n", g_gameMode);

    fprintf(file, "PLAYER1 %s %d %d %d %d %d %d\n",
        g_player1.name, g_player1.faction, g_player1.coins,
        g_player1.baseRow, g_player1.baseCol, g_player1.numBuildings, g_player1.numUnits);

    fprintf(file, "PLAYER2 %s %d %d %d %d %d %d\n",
        g_player2.name, g_player2.faction, g_player2.coins,
        g_player2.baseRow, g_player2.baseCol, g_player2.numBuildings, g_player2.numUnits);

    fprintf(file, "%s\n", BOARD_START);
    for (i = 0; i < ROWS; i++) {
        for (j = 0; j < COLS; j++) {
            fprintf(file, "%c", g_board[i][j]);
        }
        fprintf(file, "\n");
    }
    fprintf(file, "%s\n", BOARD_END);

    fprintf(file, "%s\n", P1_BUILDINGS_START);
    buildingCurrent = g_player1Buildings;
    while (buildingCurrent != NULL) {
        fprintf(file, "%d %s %d %d %d %d %c %d %d %d\n",
            buildingCurrent->building.type, buildingCurrent->building.name,
            buildingCurrent->building.width, buildingCurrent->building.height,
            buildingCurrent->building.cost, buildingCurrent->building.health,
            buildingCurrent->building.symbol, buildingCurrent->building.row,
            buildingCurrent->building.col, buildingCurrent->building.owner);
        buildingCurrent = buildingCurrent->next;
    }
    fprintf(file, "%s\n", P1_BUILDINGS_END);

    fprintf(file, "%s\n", P2_BUILDINGS_START);
    buildingCurrent = g_player2Buildings;
    while (buildingCurrent != NULL) {
        fprintf(file, "%d %s %d %d %d %d %c %d %d %d\n",
            buildingCurrent->building.type, buildingCurrent->building.name,
            buildingCurrent->building.width, buildingCurrent->building.height,
            buildingCurrent->building.cost, buildingCurrent->building.health,
            buildingCurrent->building.symbol, buildingCurrent->building.row,
            buildingCurrent->building.col, buildingCurrent->building.owner);
        buildingCurrent = buildingCurrent->next;
    }
    fprintf(file, "%s\n", P2_BUILDINGS_END);

    fprintf(file, "%s\n", P1_UNITS_START);
    unitCurrent = g_player1Units;
    while (unitCurrent != NULL) {
        fprintf(file, "%d %s %d %d %d %d %d %c %d %d %d\n",
            unitCurrent->unit.type, unitCurrent->unit.name,
            unitCurrent->unit.attackPower, unitCurrent->unit.health,
            unitCurrent->unit.moveCost, unitCurrent->unit.range,
            unitCurrent->unit.cost, unitCurrent->unit.symbol,
            unitCurrent->unit.row, unitCurrent->unit.col, unitCurrent->unit.owner);
        unitCurrent = unitCurrent->next;
    }
    fprintf(file, "%s\n", P1_UNITS_END);

    fprintf(file, "%s\n", P2_UNITS_START);
    unitCurrent = g_player2Units;
    while (unitCurrent != NULL) {
        fprintf(file, "%d %s %d %d %d %d %d %c %d %d %d\n",
            unitCurrent->unit.type, unitCurrent->unit.name,
            unitCurrent->unit.attackPower, unitCurrent->unit.health,
            unitCurrent->unit.moveCost, unitCurrent->unit.range,
            unitCurrent->unit.cost, unitCurrent->unit.symbol,
            unitCurrent->unit.row, unitCurrent->unit.col, unitCurrent->unit.owner);
        unitCurrent = unitCurrent->next;
    }
    fprintf(file, "%s\n", P2_UNITS_END);

    fprintf(file, "%s\n", SAVE_MARKER_END);
    fclose(file);

    printf("\n");
    printBoxTop();
    printBoxEmpty();
    printBoxCenter("G A M E   S A V E D");
    printBoxEmpty();
    printBoxBottom();
    printf("\n      File: %s\n", fullPath);
    printf("      Mode: %s\n", g_gameMode == 1 ? "PvP" : "PvE (vs AI)");
    printf("      Player 1: %s (%d coins)\n", g_player1.name, g_player1.coins);
    printf("      Player 2: %s (%d coins)\n", g_player2.name, g_player2.coins);
    printf("\n");
}

int loadGame(const char* filename) {
    FILE* file;
    char fullPath[256];
    char marker[50];
    char line[256];
    int i, j;
    Unit loadedUnit;
    Building loadedBuilding;

    marker[0] = '\0';

    snprintf(fullPath, sizeof(fullPath), "%s/%s", SAVES_DIR, filename);

    file = fopen(fullPath, "r");
    if (file == NULL) {
        printf("Failed to load game: file '%s' not found.\n", fullPath);
        return 0;
    }

    if (fscanf(file, "%49s", marker) != 1 || strcmp(marker, SAVE_MARKER_START) != 0) {
        printf("Failed to load game: invalid save file format.\n");
        fclose(file);
        return 0;
    }

    freeUnitList(&g_player1Units);
    freeUnitList(&g_player2Units);
    freeBuildingList(&g_player1Buildings);
    freeBuildingList(&g_player2Buildings);

    if (fscanf(file, " CURRENT_PLAYER %d", &g_currentPlayerNum) != 1) {
        fclose(file);
        return 0;
    }

    if (fscanf(file, " GAME_MODE %d", &g_gameMode) != 1) {
        g_gameMode = 1;
    }

    if (fscanf(file, " PLAYER1 %49s %d %d %d %d %d %d",
        g_player1.name, &g_player1.faction, &g_player1.coins,
        &g_player1.baseRow, &g_player1.baseCol,
        &g_player1.numBuildings, &g_player1.numUnits) != 7) {
        fclose(file);
        return 0;
    }

    if (fscanf(file, " PLAYER2 %49s %d %d %d %d %d %d",
        g_player2.name, &g_player2.faction, &g_player2.coins,
        &g_player2.baseRow, &g_player2.baseCol,
        &g_player2.numBuildings, &g_player2.numUnits) != 7) {
        fclose(file);
        return 0;
    }

    if (fscanf(file, " %49s", marker) != 1 || strcmp(marker, BOARD_START) != 0) {
        fclose(file);
        return 0;
    }

    for (i = 0; i < ROWS; i++) {
        if (fgets(line, sizeof(line), file) == NULL) {
            fclose(file);
            return 0;
        }
        if (line[0] == '\n') {
            if (fgets(line, sizeof(line), file) == NULL) {
                fclose(file);
                return 0;
            }
        }
        for (j = 0; j < COLS && line[j] != '\0' && line[j] != '\n'; j++) {
            g_board[i][j] = line[j];
        }
    }

    fscanf(file, " %49s", marker);
    fscanf(file, " %49s", marker);

    int numP1Buildings = g_player1.numBuildings;
    g_player1.numBuildings = 0;
    for (i = 0; i < numP1Buildings; i++) {
        if (fscanf(file, " %d %19s %d %d %d %d %c %d %d %d",
            &loadedBuilding.type, loadedBuilding.name,
            &loadedBuilding.width, &loadedBuilding.height,
            &loadedBuilding.cost, &loadedBuilding.health,
            &loadedBuilding.symbol, &loadedBuilding.row,
            &loadedBuilding.col, &loadedBuilding.owner) == 10) {
            addBuildingToList(&g_player1Buildings, &loadedBuilding);
            g_player1.numBuildings++;
        }
    }

    fscanf(file, " %49s", marker);
    fscanf(file, " %49s", marker);

    int numP2Buildings = g_player2.numBuildings;
    g_player2.numBuildings = 0;
    for (i = 0; i < numP2Buildings; i++) {
        if (fscanf(file, " %d %19s %d %d %d %d %c %d %d %d",
            &loadedBuilding.type, loadedBuilding.name,
            &loadedBuilding.width, &loadedBuilding.height,
            &loadedBuilding.cost, &loadedBuilding.health,
            &loadedBuilding.symbol, &loadedBuilding.row,
            &loadedBuilding.col, &loadedBuilding.owner) == 10) {
            addBuildingToList(&g_player2Buildings, &loadedBuilding);
            g_player2.numBuildings++;
        }
    }

    fscanf(file, " %49s", marker);
    fscanf(file, " %49s", marker);

    int numP1Units = g_player1.numUnits;
    g_player1.numUnits = 0;
    for (i = 0; i < numP1Units; i++) {
        if (fscanf(file, " %d %19s %d %d %d %d %d %c %d %d %d",
            &loadedUnit.type, loadedUnit.name,
            &loadedUnit.attackPower, &loadedUnit.health,
            &loadedUnit.moveCost, &loadedUnit.range,
            &loadedUnit.cost, &loadedUnit.symbol,
            &loadedUnit.row, &loadedUnit.col, &loadedUnit.owner) == 11) {
            addUnitToList(&g_player1Units, &loadedUnit);
            g_player1.numUnits++;
        }
    }

    fscanf(file, " %49s", marker);
    fscanf(file, " %49s", marker);

    int numP2Units = g_player2.numUnits;
    g_player2.numUnits = 0;
    for (i = 0; i < numP2Units; i++) {
        if (fscanf(file, " %d %19s %d %d %d %d %d %c %d %d %d",
            &loadedUnit.type, loadedUnit.name,
            &loadedUnit.attackPower, &loadedUnit.health,
            &loadedUnit.moveCost, &loadedUnit.range,
            &loadedUnit.cost, &loadedUnit.symbol,
            &loadedUnit.row, &loadedUnit.col, &loadedUnit.owner) == 11) {
            addUnitToList(&g_player2Units, &loadedUnit);
            g_player2.numUnits++;
        }
    }

    fclose(file);

    printf("\n");
    printBoxTop();
    printBoxEmpty();
    printBoxCenter("G A M E   L O A D E D");
    printBoxEmpty();
    printBoxBottom();
    printf("\n      File: %s\n", fullPath);
    printf("      Mode: %s\n", g_gameMode == 1 ? "PvP" : "PvE (vs AI)");
    printf("      Player 1: %s (%d coins)\n", g_player1.name, g_player1.coins);
    printf("      Player 2: %s (%d coins)\n", g_player2.name, g_player2.coins);
    printf("\n");

    return 1;
}

void saveGameMenu(void) {
    char filename[MAX_FILENAME];
    char fullFilename[MAX_FILENAME + 5];
    int i;

    clearScreen();
    printBoxTop();
    printBoxEmpty();
    printBoxCenter("S A V E   G A M E");
    printBoxEmpty();
    printBoxBottom();

    printf("\n      Enter save name (without .txt): ");
    if (scanf("%49s", filename) != 1) {
        clearInputBuffer();
        printf("      Invalid filename.\n");
        waitForEnter();
        return;
    }
    clearInputBuffer();

    for (i = 0; filename[i]; i++) {
        if (filename[i] == ' ') filename[i] = '_';
    }

    snprintf(fullFilename, sizeof(fullFilename), "%s.txt", filename);

    saveGame(fullFilename);
    waitForEnter();
}

static void deleteSaveFile(const char* filename) {
    char fullPath[256];
    char confirm;

    snprintf(fullPath, sizeof(fullPath), "%s/%s", SAVES_DIR, filename);

    printf("\n      Are you sure you want to delete '%s'? (y/n): ", filename);
    scanf(" %c", &confirm);
    clearInputBuffer();

    if (confirm == 'y' || confirm == 'Y') {
        if (remove(fullPath) == 0) {
            printf("      Save file deleted successfully.\n");
        }
        else {
            printf("      Error: Could not delete file.\n");
        }
    }
    else {
        printf("      Delete cancelled.\n");
    }
}

int loadGameMenu(void) {
    char saveFiles[MAX_SAVES][MAX_FILENAME];
    int numFiles = 0;
    int choice = 0;
    int i;
    char input[10];

    while (1) {
        clearScreen();
        printBoxTop();
        printBoxEmpty();
        printBoxCenter("L O A D   G A M E");
        printBoxEmpty();
        printBoxSeparator();
        printBoxEmpty();

        numFiles = listSaveFiles(saveFiles, MAX_SAVES);

        if (numFiles == 0) {
            printBoxCenter("No save files found.");
            printBoxEmpty();
            printBoxCenter("Start a new game first!");
            printBoxEmpty();
            printBoxBottom();
            printf("\n");
            waitForEnter();
            return 0;
        }

        for (i = 0; i < numFiles; i++) {
            char line[70];
            snprintf(line, sizeof(line), "[%d]  %s", i + 1, saveFiles[i]);
            printf("    |      %-62s|\n", line);
        }

        printBoxEmpty();
        printBoxSeparator();
        printBoxEmpty();
        printf("    |                       [D]  Delete a save file                       |\n");
        printf("    |                       [0]  Back to main menu                        |\n");
        printBoxEmpty();
        printBoxBottom();

        printf("\n      Enter choice: ");

        if (scanf("%9s", input) != 1) {
            clearInputBuffer();
            continue;
        }
        clearInputBuffer();

        if (input[0] == 'd' || input[0] == 'D') {
            printf("      Enter number of save to delete (1-%d): ", numFiles);
            if (scanf("%d", &choice) == 1 && choice >= 1 && choice <= numFiles) {
                clearInputBuffer();
                deleteSaveFile(saveFiles[choice - 1]);
                waitForEnter();
            }
            else {
                clearInputBuffer();
                printf("      Invalid choice.\n");
                waitForEnter();
            }
            continue;
        }

        choice = atoi(input);
        if (choice == 0) {
            return 0;
        }

        if (choice >= 1 && choice <= numFiles) {
            if (loadGame(saveFiles[choice - 1])) {
                waitForEnter();
                return 1;
            }
            else {
                waitForEnter();
            }
        }
        else {
            printf("      Invalid choice. Please try again.\n");
            waitForEnter();
        }
    }
}