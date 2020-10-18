#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned short csum(unsigned short *ptr,int nbytes) 
{
	register long sum;
	unsigned short oddbyte;
	register short answer;

	sum=0;
	while(nbytes>1) {
		sum+=*ptr++;
		nbytes-=2;
	}
	if(nbytes==1) {
		oddbyte=0;
		*((u_char*)&oddbyte)=*(u_char*)ptr;
		sum+=oddbyte;
	}

	sum = (sum>>16)+(sum & 0xffff);
	sum = sum + (sum>>16);
	answer=(short)~sum;
	
	return(answer);
}

/*
Generate a random IPv4 Address
*/
void random_ip_gen(char * ip, char ipClass){
    int lowerBound, upperBound;
	if (ipClass == 'A'){
        lowerBound = 1;
        upperBound = 126;
    }
    else if (ipClass == 'B'){
        lowerBound = 128;
        upperBound = 191;
    }
    else if (ipClass == 'C'){
        lowerBound = 192;
        upperBound = 223;
    }
    else if (ipClass == 'D'){
        lowerBound = 224;
        upperBound = 239;
    }
    else {
        printf ("Failed to generate ips for class %c\n", ipClass);
        exit(1);
    }
    //generate first octet
    int oc = (rand() % (upperBound - lowerBound + 1)) + lowerBound;
    sprintf (ip, "%d.", oc);

    //fill next three octets
    lowerBound = 0;
    upperBound = 256;
    for (int i = 1; i <= 3; i++){
        oc = rand() % upperBound;
        sprintf (ip + strlen(ip), "%d.", oc);
    }
    ip[strlen(ip) - 1] = 0; //removing the last '.'
}