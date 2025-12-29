#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>

#define ROWS 17
#define COLS 26

typedef struct {
	int type;
	int width;
	int height;
	int cost;
	int health;
	char symbol;
	int row, col;
	int owner;
} Building;

typedef struct {
	int type; // Unique unit identifier (0 = Swordsman, ...)
	int attackPower; // Damage caused on enemy units and buildings
	int health; // Health points before destruction
	int moveCost; // Coins spent per cell of movement
	int range; // Attack range in cells (Manhattan distance)
	int cost; // Recruitment cost in coins
	char symbol; // Character to draw the unit on the board
	int row, col; // Current board position
	int owner; // -1 for catalog, 0 or 1 for players’ units
} Unit;


int startGame() {
	int option = 0;

	printf(" --------------------- \n");
	printf(" | 1. Start New Game | \n");
	printf(" | 2. Load Game      | \n");
	printf(" | 3. Exit           | \n");
	printf(" --------------------- \n");

	while (option != 1 && option != 2 && option != 3) {
		printf("\n Select Option: ");
		scanf("%d", &option);

		if (option != 1 && option != 2 && option != 3) {
			printf("\n (Invalid option) \n");
		}
	}

	return option;
}


void createCatalog(Building catalog[6]) {
	catalog[0] = (Building){ 0, 3, 3, 0, 200, 'B', 0, 0, -1 };
	catalog[1] = (Building){ 1, 2, 2, 30, 60, 'M', 0, 0, -1 };
	catalog[2] = (Building){ 2, 3, 2, 50, 100, 'R', 0, 0, -1 };
	catalog[3] = (Building){ 3, 3, 2, 40, 80, 'A', 0, 0, -1 };
	catalog[4] = (Building){ 4, 3, 2, 60, 120, 'S', 0, 0, -1 };
	catalog[5] = (Building){ 5, 3, 2, 70, 140, 'W', 0, 0, -1 };
}

enum BuildingType { BASE = 0, BARRACKS = 1, RANGERY = 2, STABLE = 3, SIEGE = 4, MISC = 5 };

int unitRequiredBuilding[5] = {
	BARRACKS,
	BARRACKS,
	RANGERY,
	STABLE,
	SIEGE
};

void createUnitTypes(Unit types[5]) {
	types[0] = (Unit){ 0, 15, 40, 1, 1, 20, 'S', 0, 0, -1 };
	types[1] = (Unit){ 1, 18, 45, 1, 1, 25, 'P', 0, 0, -1 };
	types[2] = (Unit){ 2, 12, 35, 2, 2, 30, 'A', 0, 0, -1 };
	types[3] = (Unit){ 3, 20, 70, 1, 1, 40, 'K', 0, 0, -1 };
	types[4] = (Unit){ 4, 35, 50, 3, 3, 60, 'C', 0, 0, -1 };
}


void printBoard(char board[ROWS][COLS]) {
	int row, col;

	printf("   |");
	for (col = 0; col < COLS; col++) {
		printf("%2d ", col);
	}
	printf("\n");

	printf("   +");
	for (col = 0; col < COLS; col++) {
		printf("---");
	}
	printf("-\n");

	for (row = 0; row < ROWS; row++) {
		printf("%2d |", row);
		for (col = 0; col < COLS; col++) {
			printf(" %c ", board[row][col]);
		}
		printf("\n");
	}
}



int canPlaceBuilding(char board[ROWS][COLS], Building b, int r, int c) {
	if (r < 0 || c < 0 || r + b.height > ROWS || c + b.width > COLS)
		return 0; // ultrapassa borda

	for (int i = 0; i < b.height; i++)
		for (int j = 0; j < b.width; j++)
			if (board[r + i][c + j] != '.')
				return 0; // já ocupado

	return 1;
}


void placeBuilding(char board[ROWS][COLS], Building* b, int row, int col) {
	for (int i = 0; i < b->height; i++)
		for (int j = 0; j < b->width; j++)
			board[row + i][col + j] = b->symbol;
	b->row = row;
	b->col = col;
}

int placeUnit(char board[ROWS][COLS], Unit* u, int row, int col, int* playerCoins) {
	// Verifica se a posição está dentro do tabuleiro
	if (row < 0 || row >= ROWS || col < 0 || col >= COLS) {
		printf("Invalid placement: position outside the board.\n");
		return 0;
	}

	// Verifica se a célula está vazia
	if (board[row][col] != '.') {
		printf("Invalid placement: cell already occupied.\n");
		return 0;
	}

	// Verifica se o jogador tem moedas suficientes
	if (u->cost > *playerCoins) {
		printf("Not enough coins to place this unit.\n");
		return 0;
	}

	// Deduz o custo
	*playerCoins -= u->cost;

	// Atualiza o tabuleiro
	board[row][col] = u->symbol;

	// Atualiza posição da unidade
	u->row = row;
	u->col = col;

	// Confirma colocação
	printf("Unit placed successfully! Remaining coins: %d\n", *playerCoins);
	printBoard(board);

	return 1;
}

void recruitUnit(char board[ROWS][COLS], Unit units[5], int* coins, Building catalog[6], char* playerName) {
	int choice, row, col;
	char cont = 'y';

	while (cont == 'y' || cont == 'Y') {
		printf("\n=== %s's Recruit Menu ===\n", playerName);
		printf("Coins: %d\n", *coins);

		// Mostrar catálogo de unidades
		printf("\n =| UNIT CATALOG |= \n");
		for (int i = 0; i < 5; i++) {
			printf("%d: %c | Cost=%d | Health=%d | Attack=%d | Range=%d | Move=%d | Requires: ",
				i, units[i].symbol, units[i].cost, units[i].health, units[i].attackPower,
				units[i].range, units[i].moveCost);

			switch (unitRequiredBuilding[i]) {
			case BARRACKS: printf("Barracks\n"); break;
			case RANGERY:  printf("Archery Range\n"); break;
			case STABLE:   printf("Stable\n"); break;
			case SIEGE:    printf("Siege Workshop\n"); break;
			default:       printf("Unknown\n"); break;
			}
		}

		// Escolher unidade
		printf("\nSelect unit type to recruit: ");
		if (scanf("%d", &choice) != 1) {
			printf("Invalid input!\n");
			while (getchar() != '\n'); // limpa buffer
			continue;
		}

		if (choice < 0 || choice > 4) {
			printf("Invalid choice!\n");
			continue;
		}

		// Verificar se o jogador possui o edifício necessário
		int hasRequiredBuilding = 0;
		for (int i = 0; i < 6; i++) {
			if (catalog[i].owner == 0 && catalog[i].type == unitRequiredBuilding[choice]) {
				hasRequiredBuilding = 1;
				break;
			}
		}
		if (!hasRequiredBuilding) {
			printf("Cannot recruit %c: you do not have the required building.\n", units[choice].symbol);
			continue;
		}

		// Pedir coordenadas
		printf("Enter row and column to place unit: ");
		if (scanf("%d %d", &row, &col) != 2) {
			printf("Invalid input!\n");
			while (getchar() != '\n'); // limpa buffer
			continue;
		}

		// Tentar colocar unidade
		if (!placeUnit(board, &units[choice], row, col, coins)) {
			// placeUnit já imprime mensagem de erro
			continue;
		}

		printf("Unit recruited! Remaining coins: %d\n", *coins);
		printBoard(board);

		printf("Do you want to recruit another unit? (y/n): ");
		scanf(" %c", &cont);
	}
}


// ======== Build menu ========
void buildMenu(char board[ROWS][COLS], Building catalog[6], int* coins, char* playerName) {
	int choice, row, col;
	char cont = 'y';

	while (cont == 'y' || cont == 'Y') {
		printf("\n=== %s's Build Menu ===\n", playerName);
		printf("Coins: %d\n", *coins);

		printf("\n =| BUILDINGS CATALOG |= \n");
		for (int i = 0; i < 6; i++)
			printf("%d: %c (%dx%d) Cost=%d Health=%d\n",
				i, catalog[i].symbol, catalog[i].width, catalog[i].height,
				catalog[i].cost, catalog[i].health);

		printf("\nSelect building type to construct: ");
		scanf("%d", &choice);

		if (choice < 0 || choice > 5) {
			printf("Invalid building choice!\n");
			continue;
		}

		if (catalog[choice].cost > *coins) {
			printf("Not enough coins!\n");
			continue;
		}

		printf("Enter row and column (upper-left corner): ");
		scanf("%d %d", &row, &col);

		if (!canPlaceBuilding(board, catalog[choice], row, col)) {
			printf("Invalid position: building exceeds board or cells occupied.\n");
			continue;
		}

		// Place building
		placeBuilding(board, &catalog[choice], row, col);
		*coins -= catalog[choice].cost;
		printf("Building placed! Remaining coins: %d\n", *coins);
		printBoard(board);

		printf("Do you want to build another? (y/n): ");
		scanf(" %c", &cont);
	}
}

// ======== MAIN ========
int main() {
	int option;
	option = startGame();

	switch (option) {
	case 1: {
		printf("\nStarting new game...\n\n");

		char player1Name[25], player2Name[25];
		char player1Faction[25], player2Faction[25];
		int player1FactionChoice = 0;
		int player1Coins = 100, player2Coins = 100;

		// Player names
		printf(" =| PLAYER NAMES |= \n");
		printf("Player 1 name: ");
		scanf("%s", player1Name);
		printf("Player 2 name: ");
		scanf("%s", player2Name);

		// Player factions
		printf("\n =| PLAYER FACTIONS |= \n");
		printf("1. Free Peoples (Gondor / Rivendell)\n");
		printf("2. Mordor (Sauron's forces)\n\n");

		while (player1FactionChoice != 1 && player1FactionChoice != 2) {
			printf("Choose %s's Faction: ", player1Name);
			if (scanf("%d", &player1FactionChoice) != 1) {
				while (getchar() != '\n'); // limpar buffer
				continue;
			}
		}

		if (player1FactionChoice == 1) {
			strcpy(player1Faction, "Free Peoples");
			strcpy(player2Faction, "Mordor");
		}
		else {
			strcpy(player1Faction, "Mordor");
			strcpy(player2Faction, "Free Peoples");
		}

		printf("\nGiving 100 coins to each player...\n\n");

		// Summary
		printf(" =| PLAYERS SUMMARY |= \n\n");
		printf("Player 1: %s\nFaction: %s\nCoins: %d\n\n",
			player1Name, player1Faction, player1Coins);
		printf("Player 2: %s\nFaction: %s\nCoins: %d\n\n",
			player2Name, player2Faction, player2Coins);

		// Initialize board
		char board[ROWS][COLS];
		for (int i = 0; i < ROWS; i++)
			for (int j = 0; j < COLS; j++)
				board[i][j] = '.';

		printBoard(board);

		// Create catalogs
		Building catalogArray[6];
		createCatalog(catalogArray);

		Unit unitArray[5];
		createUnitTypes(unitArray);

		// Player building phase
		buildMenu(board, catalogArray, &player1Coins, player1Name);
		buildMenu(board, catalogArray, &player2Coins, player2Name);

		// Player recruitment phase
		printf("\n=== Player 1 Recruit Units ===\n");
		recruitUnit(board, unitArray, &player1Coins, catalogArray, player1Name);

		printf("\n=== Player 2 Recruit Units ===\n");
		recruitUnit(board, unitArray, &player2Coins, catalogArray, player2Name);

		printf("\nGame setup complete!\n\n");

		break;
	}
	case 2:
		printf("\nLoad game feature not implemented yet...\n\n");
		break;
	case 3:
		printf("\nLeaving...\n\n");
		break;
	}

	printf("Press Enter to exit...");
	getchar(); // absorve enter pendente
	getchar(); // espera pelo enter
	return 0;
}
