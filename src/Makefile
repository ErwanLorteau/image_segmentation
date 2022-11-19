all: segmentation segmentation_v2

CC=gcc
CFLAGS=-g -Wall

segmentation: segmentation.o Util.o
	$(CC) $^ -o $@ -lm

segmentation_v2: segmentation_v2.o Util.o
	$(CC) $^ -o $@ -lm

segmentation_v2.o: segmentation_v2.c segmentation.h Util.h
	$(CC) -c $(CFLAGS) $<

segmentation.o: segmentation.c segmentation.h Util.h
	$(CC) -c $(CFLAGS) $<

Util.o: Util.c Util.h
	$(CC) -c $(CFLAGS) $<

clean: 
	rm -f segmentation *.o