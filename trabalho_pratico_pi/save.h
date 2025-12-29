#ifndef SAVE_H
#define SAVE_H

#define DEFAULT_SAVE_FILE "savegame.txt"

void saveGame(const char* filename);
int loadGame(const char* filename);

#endif