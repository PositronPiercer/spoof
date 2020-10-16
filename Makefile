CC = gcc

all : comp


comp : 
	$(CC) -o spoof main.c dns_reflection.c utilities.c