#ifndef GLOBALS_H
#define GLOBALS_H

#include "board.h"
#include "player.h"
#include "building.h"
#include "unit.h"

extern char g_board[ROWS][COLS];
extern Player g_player1;
extern Player g_player2;

extern Building g_buildingCatalog[NUM_BUILDING_TYPES];
extern BuildingNode* g_player1Buildings;
extern BuildingNode* g_player2Buildings;

extern Unit g_unitCatalog[NUM_UNIT_TYPES];
extern UnitRequirement g_unitRequirements[NUM_UNIT_TYPES];
extern UnitNode* g_player1Units;
extern UnitNode* g_player2Units;

extern int g_currentPlayerNum;
extern int g_gameMode;

extern DestroyedNode* g_player1Kills;
extern DestroyedNode* g_player2Kills;

#endif