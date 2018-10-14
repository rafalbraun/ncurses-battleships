//int SHIP_SIZE = 4;
int STEP_VERTICAL = 2;
int STEP_HORIZONTAL = 4;
int STARTX = 5;
int STARTY = 5;
int BOARD_SIZE = 10;

int HEIGHT, WIDTH;

typedef enum SHIP_STATE {
	NOTHING = 0,
	PLACED = 1,
	SURROUND = 2,
	MISSED,
	HIT
} SHIP_STATE;

typedef enum ORIENTATION {
	VERTICAL,
	HORIZONTAL
} ORIENTATION;

typedef struct CURSOR {
	int cx;
	int cy;
	ORIENTATION orientation;
} CURSOR;

typedef enum MODE {
	PLACING,
	SELECTING,
	NONE
} MODE;