CC = gcc
CFLAGS = -Wall -g

all: wordcount 

clean:
	rm -f *.o wordcount

wordcount: wordcount.c
	$(CC) $(CFLAGS) wordcount.c -o wordcount

