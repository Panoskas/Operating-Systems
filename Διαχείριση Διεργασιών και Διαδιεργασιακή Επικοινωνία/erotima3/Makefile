all:tree2

tree2: manualtree.o proc-common.o tree.o
        gcc -Wall -o tree2 manualtree.o proc-common.o tree.o

manualtree.o:manualtree.c
        gcc -Wall -c manualtree.c


proc-common.o:proc-common.c
        gcc -Wall -c proc-common.c

tree.o:tree.c
        gcc -Wall -c tree.c

clean:
        rm manualtree.o proc-common.o tree.o tree2
