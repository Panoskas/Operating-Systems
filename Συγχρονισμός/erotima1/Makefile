#
# Makefile
# 

CC = gcc

# CAUTION: Always use '-pthread' when compiling POSIX threads-based
# applications, instead of linking with "-lpthread" directly.
CFLAGS = -Wall -O2 -pthread
LIBS =

all: pthread-test simplesync-mutex simplesync-atomic

## Pthread test
pthread-test: pthread-test.o
        $(CC) $(CFLAGS) -o pthread-test pthread-test.o $(LIBS)

pthread-test.o: pthread-test.c
        $(CC) $(CFLAGS) -c -o pthread-test.o pthread-test.c

## Simple sync (two versions)
simplesync-mutex: simplesync-mutex.o
        $(CC) $(CFLAGS) -o simplesync-mutex simplesync-mutex.o $(LIBS)

simplesync-atomic: simplesync-atomic.o
        $(CC) $(CFLAGS) -o simplesync-atomic simplesync-atomic.o $(LIBS)

simplesync-mutex.o: simplesync.c
        $(CC) $(CFLAGS) -DSYNC_MUTEX -c -o simplesync-mutex.o simplesync.c

simplesync-atomic.o: simplesync.c
        $(CC) $(CFLAGS) -DSYNC_ATOMIC -c -o simplesync-atomic.o simplesync.c

##simplesync-atomic1:simplesync.c
##      $(CC) $(CFLAGS) -DSYNC_ATOMIC simplesync.c -S -g -o simplesync-atomic.S

##simplesync-mutex1:simplesync.c
##      $(CC) $(CFLAGS) -DSYNC_MUTEX simplesync.c -S -g -o simplesync-mutex.S


clean:
        rm -f *.s *.o pthread-test simplesync-{atomic,mutex}  
