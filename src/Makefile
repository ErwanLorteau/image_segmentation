all: segmentation

CC=gcc
CFLAGS=-g -Wall

segmentation: segmentation.o Util.o
	$(CC) $^ -o $@ -lm

filter.o: segmentation.c segmentation.h Util.h
	$(CC) -c $(CFLAGS) $<

Util.o: Util.c Util.h
	$(CC) -c $(CFLAGS) $<

clean: 
	rm -f segmentation *.o