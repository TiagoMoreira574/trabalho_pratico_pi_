#ifndef BOARD_H
#define BOARD_H

#define ROWS 17
#define COLS 26
#define EMPTY_CELL '.'
#define BASE_CELL 'B'

void clearScreen(void);
void initBoard(char board[ROWS][COLS]);
void printBoard(char board[ROWS][COLS]);

#endif