all: a.out

a.out: battleships.c
	gcc battleships.c -o a.out -lncurses -lmenu
