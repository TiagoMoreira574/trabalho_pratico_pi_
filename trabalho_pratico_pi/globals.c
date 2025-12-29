#include <stdlib.h>
#include "globals.h"

char g_board[ROWS][COLS];
Player g_player1;
Player g_player2;

Building g_buildingCatalog[NUM_BUILDING_TYPES];
BuildingNode* g_player1Buildings = NULL;
BuildingNode* g_player2Buildings = NULL;

Unit g_unitCatalog[NUM_UNIT_TYPES];
UnitRequirement g_unitRequirements[NUM_UNIT_TYPES];
UnitNode* g_player1Units = NULL;
UnitNode* g_player2Units = NULL;

int g_currentPlayerNum = 0;
int g_gameMode = 1;

DestroyedNode* g_player1Kills = NULL;
DestroyedNode* g_player2Kills = NULL;