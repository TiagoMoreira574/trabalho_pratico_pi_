#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include "player.h"

void initPlayer(Player* player, const char* name, int faction, int coins) {
    strncpy(player->name, name, 49);
    player->name[49] = '\0';
    player->faction = faction;
    player->coins = coins;
    player->baseRow = 0;
    player->baseCol = 0;
    player->numBuildings = 0;
    player->numUnits = 0;
}

const char* getFactionName(int faction) {
    switch (faction) {
    case FACTION_FREE_PEOPLES: return "Free Peoples";
    case FACTION_MORDOR: return "Mordor";
    default: return "Unknown";
    }
}