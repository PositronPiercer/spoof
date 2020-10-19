
#include<stdio.h> //printf
#include<string.h>    //strlen
#include<stdlib.h>    //malloc
#include<sys/socket.h>    //you know what this is for
#include<arpa/inet.h> //inet_addr , inet_ntoa , ntohs etc
#include<netinet/in.h>
#include<unistd.h>    //getpid
#include "utilities.h"
#include <time.h>
 
/*
 * This will convert www.google.com to 3www6google3com 
 * */
void ChangetoDnsNameFormat(unsigned char* dns,unsigned char* host) 
{
    int lock = 0 , i;
    strcat((char*)host,".");
     
    for(i = 0 ; i < strlen((char*)host) ; i++) 
    {
        if(host[i]=='.') 
        {
            *dns++ = i-lock;
            for(;lock<i;lock++) 
            {
                *dns++=host[lock];
            }
            lock++; //or lock=i+1;
        }
    }
    *dns++='\0';
}


int dns_payload_gen(char * payload, unsigned char *host , int query_type){
    /*
    creates a dns payload and returns the length
    */
    int length = 0;
    unsigned char * buf = payload, *qname, *reader;
    int i , j , stop , s;
 
    struct sockaddr_in dest;
 
    struct DNS_HEADER *dns = NULL;
    struct QUESTION *qinfo = NULL;
  
    //Set the DNS structure to standard queries
    dns = (struct DNS_HEADER *)buf;
    time_t t;
	srand((unsigned) time(&t));
    dns->id = (unsigned short) htons(rand());
    dns->qr = 0; //This is a query
    dns->opcode = 0; //This is a standard query
    dns->aa = 0; //Not Authoritative
    dns->tc = 0; //This message is not truncated
    dns->rd = 1; //Recursion Desired
    dns->ra = 0; //Recursion not available! hey we dont have it (lol)
    dns->z = 0;
    dns->ad = 0;
    dns->cd = 0;
    dns->rcode = 0;
    dns->q_count = htons(1); //we have only 1 question
    dns->ans_count = 0;
    dns->auth_count = 0;
    dns->add_count = 0;
 
    //point to the query portion
    qname =(unsigned char*)(buf + sizeof(struct DNS_HEADER));
 
    ChangetoDnsNameFormat(qname , host);
    qinfo =(struct QUESTION*)(buf + sizeof(struct DNS_HEADER) + (strlen((const char*)qname) + 1)); //fill it
 
    qinfo->qtype = htons(query_type ); 
    qinfo->qclass = htons(1); 
    
    length = sizeof(struct DNS_HEADER) + (strlen((const char*)qname)+1) + sizeof(struct QUESTION);

    return length;

}

 
