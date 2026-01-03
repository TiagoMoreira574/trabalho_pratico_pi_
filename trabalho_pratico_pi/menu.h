#ifndef MENU_H
#define MENU_H

#include "board.h"
#include "player.h"
#include "building.h"
#include "unit.h"

#define MENU_NEW_GAME 1
#define MENU_LOAD_GAME 2
#define MENU_EXIT 3

#define ACTION_BUILD 1
#define ACTION_RECRUIT 2
#define ACTION_MOVE 3
#define ACTION_ATTACK 4
#define ACTION_SAVE 5
#define ACTION_END_TURN 6
#define ACTION_QUIT 7

void clearInputBuffer(void);
void waitForEnter(void);
void printBoxTop(void);
void printBoxBottom(void);
void printBoxSeparator(void);
void printBoxEmpty(void);
void printBoxCenter(const char* text);
void printBoxLeft(const char* text);

int displayMainMenu(void);
void startNewGame(Player* player1, Player* player2, char board[ROWS][COLS]);
void startNewGameVsAI(Player* player1, Player* player2, char board[ROWS][COLS]);
int displayGameMenu(Player* player);

void handleBuildAction(char board[ROWS][COLS], Player* player, Building buildingCatalog[NUM_BUILDING_TYPES], BuildingNode** playerBuildings);
void handleRecruitAction(char board[ROWS][COLS], Player* player, Unit unitCatalog[NUM_UNIT_TYPES], UnitRequirement unitRequirements[NUM_UNIT_TYPES], UnitNode** playerUnits, BuildingNode* playerBuildings);
void handleMoveAction(char board[ROWS][COLS], Player* player, UnitNode** playerUnits);
void handleAttackAction(char board[ROWS][COLS], int playerNum);

int countMinesInList(BuildingNode* head);
void awardResourceIncome(Player* player, BuildingNode* playerBuildings, int playerNum);
void endTurn(Player* player, int playerNum);

#endif