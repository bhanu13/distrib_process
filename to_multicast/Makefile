OBJS = process.o main.o
CC = g++
DEBUG = -g
CFLAGS = -Wall -c $(DEBUG)
LFLAGS = -Wall $(DEBUG)

proc: $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o proc

main.o: main.cpp process.h
	$(CC) $(CFLAGS) main.cpp

process.o: process.cpp process.h
	$(CC) $(CFLAGS) process.cpp

clean:
	\rm *.o *~ proc