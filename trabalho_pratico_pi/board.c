#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include "board.h"

void clearScreen(void) {
#ifdef _WIN32
    system("cls");
#else
    if (system("clear") == -1) {
        printf("\n\n\n");
    }
#endif
}

void initBoard(char board[ROWS][COLS]) {
    int i, j;
    for (i = 0; i < ROWS; i++) {
        for (j = 0; j < COLS; j++) {
            board[i][j] = EMPTY_CELL;
        }
    }
}

void printBoard(char board[ROWS][COLS]) {
    int i, j;
    printf("\n  +=====================================================================================+\n");
    printf("  |                                MIDDLE-EARTH MAP                                     |\n");
    printf("  +=====================================================================================+\n");
    printf("  |      ");
    for (j = 0; j < COLS; j++) {
        printf("%2d ", j);
    }
    printf(" |\n");
    printf("  |    +------------------------------------------------------------------------------+ |\n");
    for (i = 0; i < ROWS; i++) {
        printf("  | %2d |", i);
        for (j = 0; j < COLS; j++) {
            printf(" %c ", board[i][j]);
        }
        printf("| |\n");
    }
    printf("  |    +------------------------------------------------------------------------------+ |\n");
    printf("  +=====================================================================================+\n\n");
}