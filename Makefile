CC = gcc

all : comp


comp : 
	$(CC) -g -o iwnm main.c dns_reflection.c utilities.c dns.c smurf.c