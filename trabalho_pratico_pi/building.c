#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "building.h"
#include "board.h"

void initBuildingCatalog(Building catalog[NUM_BUILDING_TYPES]) {
    catalog[0].type = BUILDING_BASE; strcpy(catalog[0].name, "Base");
    catalog[0].width = 3; catalog[0].height = 3; catalog[0].cost = 0;
    catalog[0].health = 200; catalog[0].symbol = 'B'; catalog[0].owner = -1;

    catalog[1].type = BUILDING_MINE; strcpy(catalog[1].name, "Mine");
    catalog[1].width = 2; catalog[1].height = 2; catalog[1].cost = 30;
    catalog[1].health = 60; catalog[1].symbol = 'M'; catalog[1].owner = -1;

    catalog[2].type = BUILDING_BARRACKS; strcpy(catalog[2].name, "Barracks");
    catalog[2].width = 3; catalog[2].height = 2; catalog[2].cost = 50;
    catalog[2].health = 100; catalog[2].symbol = 'R'; catalog[2].owner = -1;

    catalog[3].type = BUILDING_ARCHERY_RANGE; strcpy(catalog[3].name, "ArcheryRange");
    catalog[3].width = 3; catalog[3].height = 2; catalog[3].cost = 40;
    catalog[3].health = 80; catalog[3].symbol = 'A'; catalog[3].owner = -1;

    catalog[4].type = BUILDING_STABLE; strcpy(catalog[4].name, "Stable");
    catalog[4].width = 3; catalog[4].height = 2; catalog[4].cost = 60;
    catalog[4].health = 120; catalog[4].symbol = 'S'; catalog[4].owner = -1;

    catalog[5].type = BUILDING_SIEGE_WORKSHOP; strcpy(catalog[5].name, "SiegeWorkshop");
    catalog[5].width = 3; catalog[5].height = 2; catalog[5].cost = 70;
    catalog[5].health = 140; catalog[5].symbol = 'W'; catalog[5].owner = -1;
}

void displayBuildingCatalog(Building catalog[NUM_BUILDING_TYPES]) {
    int i;
    printf("\n==========================================================================\n");
    printf("                         BUILDING CATALOG                                 \n");
    printf("==========================================================================\n");
    printf(" Type | Name             | Size (WxH) | Cost | Health | Symbol           \n");
    printf("--------------------------------------------------------------------------\n");
    for (i = 0; i < NUM_BUILDING_TYPES; i++) {
        printf("  %d   | %-16s |    %dx%d     | %3d  |  %3d   |   %c\n",
            i, catalog[i].name, catalog[i].width, catalog[i].height,
            catalog[i].cost, catalog[i].health, catalog[i].symbol);
    }
    printf("==========================================================================\n");
    printf(" Note: Base (type 0) is placed automatically and cannot be constructed.  \n");
    printf("==========================================================================\n");
    printf("  -1  | Cancel                                                            \n");
    printf("==========================================================================\n");
}

int canPlaceBuilding(char board[ROWS][COLS], int row, int col, int width, int height) {
    int i, j;
    if (row < 0 || col < 0 || row + height > ROWS || col + width > COLS) {
        return 0;
    }
    for (i = row; i < row + height; i++) {
        for (j = col; j < col + width; j++) {
            if (board[i][j] != EMPTY_CELL) {
                return 0;
            }
        }
    }
    return 1;
}

int placeBuilding(char board[ROWS][COLS], Building* building, int row, int col) {
    int i, j;
    if (!canPlaceBuilding(board, row, col, building->width, building->height)) {
        return 0;
    }
    building->row = row;
    building->col = col;
    for (i = row; i < row + building->height; i++) {
        for (j = col; j < col + building->width; j++) {
            board[i][j] = building->symbol;
        }
    }
    return 1;
}

BuildingNode* addBuildingToList(BuildingNode** head, Building* building) {
    BuildingNode* newNode = (BuildingNode*)malloc(sizeof(BuildingNode));
    BuildingNode* current;
    if (newNode == NULL) {
        printf("Error: Failed to allocate memory for building.\n");
        return NULL;
    }
    newNode->building = *building;
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

int removeBuildingFromList(char board[ROWS][COLS], BuildingNode** head, int row, int col) {
    BuildingNode* current = *head;
    BuildingNode* prev = NULL;
    int r, c, br, bc;
    while (current != NULL) {
        for (br = current->building.row; br < current->building.row + current->building.height; br++) {
            for (bc = current->building.col; bc < current->building.col + current->building.width; bc++) {
                if (br == row && bc == col) {
                    for (r = current->building.row; r < current->building.row + current->building.height; r++) {
                        for (c = current->building.col; c < current->building.col + current->building.width; c++) {
                            if (r >= 0 && r < ROWS && c >= 0 && c < COLS) {
                                board[r][c] = EMPTY_CELL;
                            }
                        }
                    }
                    if (prev == NULL) {
                        *head = current->next;
                    }
                    else {
                        prev->next = current->next;
                    }
                    free(current);
                    return 1;
                }
            }
        }
        prev = current;
        current = current->next;
    }
    return 0;
}

BuildingNode* findBuildingInList(BuildingNode* head, int row, int col) {
    BuildingNode* current = head;
    int r, c;
    while (current != NULL) {
        for (r = current->building.row; r < current->building.row + current->building.height; r++) {
            for (c = current->building.col; c < current->building.col + current->building.width; c++) {
                if (r == row && c == col) {
                    return current;
                }
            }
        }
        current = current->next;
    }
    return NULL;
}

int countBuildingsInList(BuildingNode* head) {
    int count = 0;
    BuildingNode* current = head;
    while (current != NULL) {
        count++;
        current = current->next;
    }
    return count;
}

void freeBuildingList(BuildingNode** head) {
    BuildingNode* current = *head;
    BuildingNode* next;
    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }
    *head = NULL;
}

int hasBuildingInList(BuildingNode* head, int buildingType) {
    BuildingNode* current = head;
    while (current != NULL) {
        if (current->building.type == buildingType) {
            return 1;
        }
        current = current->next;
    }
    return 0;
}

int countBuildingTypeInList(BuildingNode* head, int buildingType) {
    int count = 0;
    BuildingNode* current = head;
    while (current != NULL) {
        if (current->building.type == buildingType) {
            count++;
        }
        current = current->next;
    }
    return count;
}