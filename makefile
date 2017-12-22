CC=gcc
CFLAGS= -c

all: sniffer

sniffer: manager.o worker.o list.o
	$(CC) -o sniffer manager.o worker.o list.o

manager.o: worker.h list.h
	$(CC) $(CFLAGS) manager.c

list.o: list.h
	$(CC) $(CFLAGS) list.c

worker.o: worker.h
	$(CC) $(CFLAGS) worker.c

clean:
	rm -f *.o

