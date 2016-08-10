CC     = gcc
CFLAGS = -O3 -g -Wall -Wextra
FILE1 = ks_0_1.c

ks :
	$(CC) $(CFLAGS) -o ks $(FILE1)

clean :
	-rm -f *.o *.core ks
