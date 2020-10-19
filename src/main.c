#include "attackers.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

int main (void)
{	
	if(getuid()){
		printf("I want to run as root!\n");
      	exit(EXIT_FAILURE);
   }
	FILE * banner = fopen ("assets/banner.txt", "r");
	fseek(banner, 0, SEEK_END);
	long fsize = ftell(banner);
	fseek(banner, 0, SEEK_SET);  /* same as rewind(f); */

	char *string = malloc(fsize + 1);
	fread(string, 1, fsize, banner);
	fclose(banner);

	//initialise random number generator
	time_t t;
	srand((unsigned) time(&t));

	int choice = 0;

	string[fsize] = 0;
	printf("%s\n", string);

	printf ("__Available Attacks__\n");
	printf ("1. DNS Reflection\n2. IP Smurf\n3. TCP SYN Flood\n");
	printf ("\nEnter choice : ");

	

	scanf ("%d", &choice);

	if (choice == 1){
		dns_reflection_setup();
	}
	else if (choice == 2){
		smurf_setup();
	}
	else if (choice == 3){
		syn_flood_setup();
	}
	
	return 0;
}