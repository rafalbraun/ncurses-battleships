all: a.out

Safe.o: Safe.hs
	ghc -c -O Safe.hs

a.out: main.c Safe.o
	ghc --make -no-hs-main -optc-O main.c Safe -o a.out

clean:
	rm a.out Safe.o