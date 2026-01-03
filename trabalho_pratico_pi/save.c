#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include "save.h"
#include "globals.h"

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

void saveGame(const char* filename) {
    FILE* file = fopen(filename, "w");
    int i, j;
    UnitNode* unitCurrent;
    BuildingNode* buildingCurrent;

    if (file == NULL) {
        printf("Error: Could not create save file '%s'.\n", filename);
        printf("Check if you have write permissions in this directory.\n");
        return;
    }

    printf("Saving game to '%s'...\n", filename);

    fprintf(file, "%s\n", SAVE_MARKER_START);
    fprintf(file, "CURRENT_PLAYER %d\n", g_currentPlayerNum);
    fprintf(file, "GAME_MODE %d\n", g_gameMode);  /* 1=PvP, 2=PvE */

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
    printf("Game saved successfully to '%s'.\n", filename);
    printf("  - Game Mode: %s\n", g_gameMode == 1 ? "PvP" : "PvE (vs AI)");
    printf("  - Current Player: %d\n", g_currentPlayerNum + 1);
    printf("  - Player 1: %s (%d coins, %d buildings, %d units)\n",
        g_player1.name, g_player1.coins, g_player1.numBuildings, g_player1.numUnits);
    printf("  - Player 2: %s (%d coins, %d buildings, %d units)\n",
        g_player2.name, g_player2.coins, g_player2.numBuildings, g_player2.numUnits);
}

int loadGame(const char* filename) {
    FILE* file = fopen(filename, "r");
    char marker[50] = { '\0' };
    char line[256];
    int i, j;
    Unit loadedUnit;
    Building loadedBuilding;

    if (file == NULL) {
        printf("Failed to load game: file not found or invalid format.\n");
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
        g_gameMode = 1;  /* Default to PvP if not found (old save files) */
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

    fscanf(file, " %49s", marker);
    if (strcmp(marker, BOARD_START) != 0) {
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
    printf("Game loaded successfully from '%s'.\n", filename);
    printf("  - Game Mode: %s\n", g_gameMode == 1 ? "PvP" : "PvE (vs AI)");
    printf("  - Current Player: %d\n", g_currentPlayerNum + 1);
    printf("  - Player 1: %s (%d coins, %d buildings, %d units)\n",
        g_player1.name, g_player1.coins, g_player1.numBuildings, g_player1.numUnits);
    printf("  - Player 2: %s (%d coins, %d buildings, %d units)\n",
        g_player2.name, g_player2.coins, g_player2.numBuildings, g_player2.numUnits);
    return 1;
}