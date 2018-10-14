all: a.out

a.out: main.c
	gcc main.c -o a.out -lncurses -lmenu

clean:
	rm a.out