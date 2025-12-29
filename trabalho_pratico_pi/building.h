#ifndef BUILDING_H
#define BUILDING_H

#include "board.h"

#define NUM_BUILDING_TYPES 6
#define MAX_PLAYER_BUILDINGS 20

#define BUILDING_BASE 0
#define BUILDING_MINE 1
#define BUILDING_BARRACKS 2
#define BUILDING_ARCHERY_RANGE 3
#define BUILDING_STABLE 4
#define BUILDING_SIEGE_WORKSHOP 5

typedef struct {
    int type;
    char name[20];
    int width;
    int height;
    int cost;
    int health;
    char symbol;
    int row;
    int col;
    int owner;
} Building;

typedef struct BuildingNode {
    Building building;
    struct BuildingNode* next;
} BuildingNode;

void initBuildingCatalog(Building catalog[NUM_BUILDING_TYPES]);
void displayBuildingCatalog(Building catalog[NUM_BUILDING_TYPES]);
int canPlaceBuilding(char board[ROWS][COLS], int row, int col, int width, int height);
int placeBuilding(char board[ROWS][COLS], Building* building, int row, int col);

BuildingNode* addBuildingToList(BuildingNode** head, Building* building);
int removeBuildingFromList(char board[ROWS][COLS], BuildingNode** head, int row, int col);
BuildingNode* findBuildingInList(BuildingNode* head, int row, int col);
int countBuildingsInList(BuildingNode* head);
void freeBuildingList(BuildingNode** head);
int hasBuildingInList(BuildingNode* head, int buildingType);
int countBuildingTypeInList(BuildingNode* head, int buildingType);

#endif