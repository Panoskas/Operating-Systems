all: tree

tree: forktree.o proc-common.o
        gcc -Wall -o tree forktree.o proc-common.o

forktree.o: forktree.c
        gcc -Wall -c forktree.c

proc-common.o: proc-common.c
        gcc -Wall -c proc-common.c

clean:
        rm proc-common.o forktree.o tree
