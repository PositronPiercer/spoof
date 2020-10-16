CC = gcc

all : comp


comp : 
	$(CC) -g -o spoof main.c dns_reflection.c utilities.c dns.c