#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include "board.h"
#include "player.h"
#include "menu.h"
#include "building.h"
#include "unit.h"
#include "save.h"
#include "ai.h"
#include "globals.h"

void displayStatistics(void) {
    clearScreen();

    printBoxTop();
    printBoxEmpty();
    printBoxCenter("G A M E   S T A T I S T I C S");
    printBoxEmpty();
    printBoxSeparator();

    char line[80];
    int p1Kills = countDestroyedEntities(g_player1Kills);
    int p2Kills = countDestroyedEntities(g_player2Kills);

    sprintf(line, "%s - Total Kills: %d", g_player1.name, p1Kills);
    printBoxCenter(line);
    printBoxEmpty();
    displayDestroyedList(g_player1Kills, 0);

    printBoxSeparator();

    sprintf(line, "%s - Total Kills: %d", g_player2.name, p2Kills);
    printBoxCenter(line);
    printBoxEmpty();
    displayDestroyedList(g_player2Kills, 1);

    printBoxBottom();
    waitForEnter();
}

void runGameLoop(int startingPlayer) {
    int gameRunning = 1, turnActive, action, isAI;
    Player* currentPlayer;
    BuildingNode** currentPlayerBuildings;
    UnitNode** currentPlayerUnits;
    char line[80];

    g_currentPlayerNum = startingPlayer;

    if (g_currentPlayerNum == 0) {
        currentPlayer = &g_player1;
        currentPlayerBuildings = &g_player1Buildings;
        currentPlayerUnits = &g_player1Units;
    }
    else {
        currentPlayer = &g_player2;
        currentPlayerBuildings = &g_player2Buildings;
        currentPlayerUnits = &g_player2Units;
    }

    while (gameRunning) {
        isAI = (g_gameMode == GAME_MODE_PVE && g_currentPlayerNum == 1);
        clearScreen();

        printBoxTop();
        printBoxEmpty();
        if (isAI) {
            printBoxCenter("A I   T U R N");
        }
        else {
            sprintf(line, "Player %d's Turn", g_currentPlayerNum + 1);
            printBoxCenter(line);
        }
        printBoxEmpty();
        printBoxBottom();

        awardResourceIncome(currentPlayer, *currentPlayerBuildings, g_currentPlayerNum);

        printf("\n      Current coins: %d\n", currentPlayer->coins);
        printBoard(g_board);

        if (isAI) {
            executeAITurn(g_board, &g_player2, &g_player1, 1, g_buildingCatalog,
                &g_player2Buildings, &g_player1Buildings, g_unitCatalog,
                g_unitRequirements, &g_player2Units, &g_player1Units);
            endTurn(currentPlayer, g_currentPlayerNum);
            currentPlayer = &g_player1;
            currentPlayerBuildings = &g_player1Buildings;
            currentPlayerUnits = &g_player1Units;
            g_currentPlayerNum = 0;
        }
        else {
            turnActive = 1;
            while (turnActive && gameRunning) {
                if (currentPlayer->coins <= 0) {
                    currentPlayer->coins = 0;
                    clearScreen();
                    printBoxTop();
                    printBoxEmpty();
                    printBoxCenter("N O   C O I N S   L E F T");
                    printBoxEmpty();
                    printBoxCenter("Your turn ends automatically.");
                    printBoxEmpty();
                    printBoxBottom();
                    waitForEnter();
                    turnActive = 0;
                    break;
                }

                action = displayGameMenu(currentPlayer);

                switch (action) {
                case ACTION_BUILD:
                    handleBuildAction(g_board, currentPlayer, g_buildingCatalog,
                        currentPlayerBuildings);
                    clearScreen();
                    printBoard(g_board);
                    break;
                case ACTION_RECRUIT:
                    handleRecruitAction(g_board, currentPlayer, g_unitCatalog,
                        g_unitRequirements, currentPlayerUnits,
                        *currentPlayerBuildings);
                    clearScreen();
                    printBoard(g_board);
                    break;
                case ACTION_MOVE:
                    handleMoveAction(g_board, currentPlayer, currentPlayerUnits);
                    clearScreen();
                    printBoard(g_board);
                    break;
                case ACTION_ATTACK:
                    handleAttackAction(g_board, g_currentPlayerNum);
                    clearScreen();
                    printBoard(g_board);
                    break;
                case ACTION_SAVE:
                    saveGame(DEFAULT_SAVE_FILE);
                    clearScreen();
                    printBoxTop();
                    printBoxEmpty();
                    printBoxCenter("G A M E   S A V E D");
                    printBoxEmpty();
                    printBoxCenter("Your progress has been saved.");
                    printBoxEmpty();
                    printBoxBottom();
                    waitForEnter();
                    clearScreen();
                    printBoard(g_board);
                    break;
                case ACTION_VIEW_STATS:
                    displayStatistics();
                    clearScreen();
                    printBoard(g_board);
                    break;
                case ACTION_END_TURN:
                    turnActive = 0;
                    break;
                case ACTION_QUIT:
                    clearScreen();
                    printBoxTop();
                    printBoxEmpty();
                    printBoxCenter("R E T U R N I N G   T O   M E N U");
                    printBoxEmpty();
                    printBoxCenter("Thank you for playing!");
                    printBoxEmpty();
                    printBoxBottom();
                    waitForEnter();
                    gameRunning = 0;
                    turnActive = 0;
                    break;
                default:
                    printf("\n      Invalid option.\n");
                    waitForEnter();
                    break;
                }
            }

            if (gameRunning) {
                endTurn(currentPlayer, g_currentPlayerNum);
                if (g_currentPlayerNum == 0) {
                    currentPlayer = &g_player2;
                    currentPlayerBuildings = &g_player2Buildings;
                    currentPlayerUnits = &g_player2Units;
                    g_currentPlayerNum = 1;
                }
                else {
                    currentPlayer = &g_player1;
                    currentPlayerBuildings = &g_player1Buildings;
                    currentPlayerUnits = &g_player1Units;
                    g_currentPlayerNum = 0;
                }
            }
        }
    }
}

int main(void) {
    int running = 1, choice;

    initBoard(g_board);
    initBuildingCatalog(g_buildingCatalog);
    initUnitCatalog(g_unitCatalog);
    initUnitRequirements(g_unitRequirements);

    while (running) {
        choice = displayMainMenu();

        switch (choice) {
        case MENU_NEW_GAME:
            g_gameMode = selectGameMode();
            initBoard(g_board);
            freeUnitList(&g_player1Units);
            freeUnitList(&g_player2Units);
            freeBuildingList(&g_player1Buildings);
            freeBuildingList(&g_player2Buildings);
            freeDestroyedList(&g_player1Kills);
            freeDestroyedList(&g_player2Kills);
            g_player1.numBuildings = 0;
            g_player1.numUnits = 0;
            g_player2.numBuildings = 0;
            g_player2.numUnits = 0;

            if (g_gameMode == GAME_MODE_PVE) {
                startNewGameVsAI(&g_player1, &g_player2, g_board);
            }
            else {
                startNewGame(&g_player1, &g_player2, g_board);
            }

            clearScreen();
            printBoxTop();
            printBoxEmpty();
            printBoxCenter("G A M E   S T A R T S");
            printBoxEmpty();
            printBoxBottom();
            printBoard(g_board);
            waitForEnter();

            runGameLoop(0);
            break;

        case MENU_LOAD_GAME:
            if (loadGame(DEFAULT_SAVE_FILE)) {
                clearScreen();
                printBoxTop();
                printBoxEmpty();
                printBoxCenter("G A M E   L O A D E D");
                printBoxEmpty();
                printBoxSeparator();

                char line[120];
                snprintf(line, sizeof(line), "%s vs %s", g_player1.name, g_player2.name);
                printBoxCenter(line);
                printBoxEmpty();
                printBoxBottom();

                printBoard(g_board);
                waitForEnter();
                runGameLoop(g_currentPlayerNum);
            }
            else {
                clearScreen();
                printBoxTop();
                printBoxEmpty();
                printBoxCenter("L O A D   F A I L E D");
                printBoxEmpty();
                printBoxCenter("No saved game found.");
                printBoxEmpty();
                printBoxBottom();
                waitForEnter();
            }
            break;

        case MENU_EXIT:
            freeUnitList(&g_player1Units);
            freeUnitList(&g_player2Units);
            freeBuildingList(&g_player1Buildings);
            freeBuildingList(&g_player2Buildings);
            freeDestroyedList(&g_player1Kills);
            freeDestroyedList(&g_player2Kills);

            clearScreen();
            printBoxTop();
            printBoxEmpty();
            printBoxCenter("F A R E W E L L");
            printBoxEmpty();
            printBoxCenter("Thank you for playing!");
            printBoxCenter("May the light of Earendil guide you.");
            printBoxEmpty();
            printBoxBottom();
            printf("\n");
            running = 0;
            break;

        default:
            break;
        }
    }
    return 0;
}