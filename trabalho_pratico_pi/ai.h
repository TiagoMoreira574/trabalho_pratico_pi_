#ifndef AI_H
#define AI_H

#include "board.h"
#include "player.h"
#include "building.h"
#include "unit.h"

#define GAME_MODE_PVP 1
#define GAME_MODE_PVE 2

int selectGameMode(void);
int aiBuildMine(char board[ROWS][COLS], Player* aiPlayer, Building buildingCatalog[NUM_BUILDING_TYPES], BuildingNode** aiBuildings);
int aiBuildBarracks(char board[ROWS][COLS], Player* aiPlayer, Building buildingCatalog[NUM_BUILDING_TYPES], BuildingNode** aiBuildings);
int aiRecruitUnit(char board[ROWS][COLS], Player* aiPlayer, Unit unitCatalog[NUM_UNIT_TYPES], UnitRequirement unitRequirements[NUM_UNIT_TYPES], UnitNode** aiUnits, BuildingNode* aiBuildings);
int aiAttack(int aiPlayerNum);
int aiMoveTowardsEnemy(char board[ROWS][COLS], Player* aiPlayer, UnitNode** aiUnits, int enemyBaseRow, int enemyBaseCol);
void executeAITurn(char board[ROWS][COLS], Player* aiPlayer, Player* humanPlayer, int aiPlayerNum, Building buildingCatalog[NUM_BUILDING_TYPES], BuildingNode** aiBuildings, BuildingNode** humanBuildings, Unit unitCatalog[NUM_UNIT_TYPES], UnitRequirement unitRequirements[NUM_UNIT_TYPES], UnitNode** aiUnits, UnitNode** humanUnits);

#endif