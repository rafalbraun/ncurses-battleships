#include <time.h>
#include <curses.h>
#include <stdlib.h>
#include <menu.h>
#include <stdlib.h>
#include <stdio.h>

int WIDTH = 10;
int HEIGHT = 10;

void save_board() {
	FILE* file = fopen("./save.out", "w");
	if (file == NULL) {
		printf("Could not open file.");
	}
	for (int i=0; i<WIDTH; i++) {
		for (int j=0; j<WIDTH; j++) {
			putc(48, file);
		}
		fputs("\n", file);
	}
	fclose(file);
	printf("File saved.\n");
}

int** allocate_board() {
	int** players_board=(int **)calloc(WIDTH, sizeof(int *));
	for (int i=0; i < WIDTH; i++) {
		players_board[i] = (int *)calloc(HEIGHT, sizeof(int));
	}
	return players_board;
}

int main() {
	int** board = allocate_board();

	for (int i=0; i < WIDTH; i++) {
		for (int j=0; j < HEIGHT; j++) {
			board[i][j] = 0;
		}
	}
}
