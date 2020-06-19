all:zing zing2


zing:main.o zing.o
        gcc -Wall main.o zing.o -o zing


main.o:main.c
        gcc -Wall -c main.c

zing2: zing2.o main.o
        gcc -Wall zing2.o main.o -o zing2

zing2.o:zing2.c
        gcc -Wall -c zing2.c
