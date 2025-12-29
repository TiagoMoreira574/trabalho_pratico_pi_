#ifndef UNIT_H
#define UNIT_H

#include "board.h"
#include "building.h"

#define NUM_UNIT_TYPES 5
#define MAX_PLAYER_UNITS 50

#define UNIT_SWORDSMAN 0
#define UNIT_SPEARMAN 1
#define UNIT_ARCHER 2
#define UNIT_KNIGHT 3
#define UNIT_CATAPULT 4

#define UNIT_OWNER_PLAYER1 0
#define UNIT_OWNER_PLAYER2 1

#define DESTROYED_UNIT 0
#define DESTROYED_BUILDING 1

typedef struct {
    int type;
    char name[20];
    int attackPower;
    int health;
    int moveCost;
    int range;
    int cost;
    char symbol;
    int row;
    int col;
    int owner;
} Unit;

typedef struct UnitNode {
    Unit unit;
    struct UnitNode* next;
} UnitNode;

typedef struct {
    int requiredBuilding;
    char buildingName[20];
} UnitRequirement;

typedef struct {
    int entityType;
    char name[20];
    int originalOwner;
    int row;
    int col;
} DestroyedEntity;

typedef struct DestroyedNode {
    DestroyedEntity entity;
    struct DestroyedNode* next;
} DestroyedNode;

void initUnitCatalog(Unit catalog[NUM_UNIT_TYPES]);
void initUnitRequirements(UnitRequirement requirements[NUM_UNIT_TYPES]);
void displayUnitCatalog(Unit catalog[NUM_UNIT_TYPES], UnitRequirement requirements[NUM_UNIT_TYPES]);
int placeUnit(char board[ROWS][COLS], Unit* u, int row, int col);

UnitNode* addUnitToList(UnitNode** head, Unit* unit);
int removeUnitFromList(char board[ROWS][COLS], UnitNode** head, int row, int col);
UnitNode* findUnitInList(UnitNode* head, int row, int col);
int countUnitsInList(UnitNode* head);
void freeUnitList(UnitNode** head);
void displayUnitList(UnitNode* head, int playerNum);

int findNearestFreeCell(char board[ROWS][COLS], int targetRow, int targetCol, int* foundRow, int* foundCol);
int moveUnit(Unit* u, int destRow, int destCol);
int attack(Unit* attacker, int targetRow, int targetCol);

DestroyedNode* addDestroyedEntity(DestroyedNode** head, int entityType, const char* name, int originalOwner, int row, int col);
int countDestroyedEntities(DestroyedNode* head);
void displayDestroyedList(DestroyedNode* head, int playerNum);
void freeDestroyedList(DestroyedNode** head);

#endif