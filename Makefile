CC = gcc

all : comp


comp : 
	$(CC) -g -o iwnm src/*.c -I include