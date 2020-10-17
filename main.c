#include "attackers.h"
#include <stdio.h>
#include <stdlib.h>


int main (void)
{	
	if(getuid()){
		printf("I want to run as root!\n");
      	exit(EXIT_FAILURE);
   }
	FILE * banner = fopen ("banner.txt", "r");
	fseek(banner, 0, SEEK_END);
	long fsize = ftell(banner);
	fseek(banner, 0, SEEK_SET);  /* same as rewind(f); */

	char *string = malloc(fsize + 1);
	fread(string, 1, fsize, banner);
	fclose(banner);

	string[fsize] = 0;
	printf("%s\n", string);
	//dns_reflection_setup();
	smurf_setup();
	
	return 0;
}