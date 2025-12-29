#ifndef PLAYER_H
#define PLAYER_H

#define FACTION_FREE_PEOPLES 1
#define FACTION_MORDOR 2

typedef struct {
    char name[50];
    int faction;
    int coins;
    int baseRow;
    int baseCol;
    int numBuildings;
    int numUnits;
} Player;

void initPlayer(Player* player, const char* name, int faction, int coins);
const char* getFactionName(int faction);

#endif