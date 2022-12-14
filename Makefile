#
# Makefile that builds btest and other helper programs for the CS:APP data lab
# 
CC = gcc
CFLAGS = -O2 -Wall -m32 -ggdb
LIBFLAGS=-pthread

all: mm mm-mt

mm: mm.c mm.h 
	$(CC) $(CFLAGS) -o mm mm.c 

clean:
	rm -f *.o mm mm-mt
	
mm-mt: 
	$(CC) $(CFLAGS) -o mm-mt mm-mt.c 
	
	

	
