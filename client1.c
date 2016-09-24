
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "client.h"


char phase3_port[20];
char phase3_server[50];
	
void *get_in_addr(struct sockaddr *sa)
{
if (sa->sa_family == AF_INET) {
return &(((struct sockaddr_in*)sa)->sin_addr);
}
return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

// the port users will be connecting to
int main()
{
	int port_num=phase2();
	printf("***************************************************************\n");
	int return_val=phase3();
	return 0;
}

int phase2()
{
	usleep(90000);
	int sockfd,sockfd2;
	struct addrinfo hints, *servinfo, *p, *client, *p2;
	int rv;
	int numbytes;
	int yes=1;
	char send_to_dir[30]="Client1 doc1\n";
	int len_send_to_dir=strlen(send_to_dir);


/*		
	if (num_arg != 2) 
	{
		fprintf(stderr,"usage: talker hostname message\n");
		exit(1);
	}
*/	
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
//	hints.ai_flags = AI_PASSIVE;
	if ((rv = getaddrinfo("127.0.0.1", DIR_SERVER, &hints, &servinfo)) != 0) 
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}
	
	
	// start bind

	if ((rv = getaddrinfo("127.0.0.1", CLIENT1, &hints, &client)) != 0) 
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return -1;
	
	}
	
	
// loop through all the results and bind to the first we can
	for(p2 = client; p2 != NULL; p2 = p2->ai_next) 
	{
		if ((sockfd2 = socket(p2->ai_family, p2->ai_socktype,
p2->ai_protocol)) == -1) 
		{
			perror("listener: socket");
			continue;
		}

// remove any exsisting bind
		if (setsockopt(sockfd2, SOL_SOCKET, SO_REUSEADDR, &yes,
sizeof(int)) == -1) 
		{
			perror("setsockopt");
			return(-1);
		}


		if (bind(sockfd2, p2->ai_addr, p2->ai_addrlen) == -1) 
		{
			close(sockfd2);
			perror("listener: bind");
			continue;
		}
		break;
		
	// end bind		
	
	}

//	 print port
	int port;
	struct sockaddr_in sinport;
	socklen_t len=sizeof(sinport);
	if(getsockname(sockfd2,(struct sockaddr *)&sinport,&len)==-1)
		perror("getsockname");
	else
		port=ntohs(sinport.sin_port);


//	end print port



//ip
/*
	void *addr;
	char f2_ip[INET6_ADDRSTRLEN];
	if (p2->ai_family == AF_INET) 
	{ // IPv4
		struct sockaddr_in *ipv4 = (struct sockaddr_in *)p2->ai_addr;
		addr = &(ipv4->sin_addr);
		
	}
	 else
	{ // IPv6
		struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p2->ai_addr;
		addr = &(ipv6->sin6_addr);
		
	}
	
	
	//ip 
	inet_ntop(p2->ai_family,addr,f2_ip, sizeof f2_ip);
	
	printf("\nPhase 2: Client1 has UDP Port Number %d and IP address %s \n", port,f2_ip);
*/

struct hostent *he;
	if ((he=gethostbyname("nunki.usc.edu")) == NULL)
	{
		perror("gethostbyname");
		exit(1);
	}
	struct sockaddr_in ip_addr;
	ip_addr.sin_addr = *((struct in_addr *)he->h_addr);
	
	
	printf("\nPhase 2: Client 1 has UDP Port Number %d and IP address %s \n",port,inet_ntoa(ip_addr.sin_addr));

//end ip





// loop through all the results and make a socket
	for(p = servinfo; p != NULL; p = p->ai_next) 
	{
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
p->ai_protocol)) == -1) 
		{
			perror("talker: socket");
			continue;
		}
		break;
	}

	if (p == NULL) 
	{
		fprintf(stderr, "talker: failed to bind socket\n");
		return 2;
	}
	
//Send Request to Directory Server
	if ((numbytes = sendto(sockfd, send_to_dir , len_send_to_dir, 0,
p->ai_addr, p->ai_addrlen)) == -1) 
	{
		perror("talker: sendto");
		exit(1);
	}
	
	
	char buf[70];

	//Receive File Server Port number from Directory Server
	if ((numbytes = recvfrom(sockfd, buf, 69 , 0,p->ai_addr, &(p->ai_addrlen))) == -1) 
	{
		perror("recvfrom");
		return(-1);
	}
	
	buf[numbytes]='\0';
	
//	printf("Received from Server %s \n",buf);
	
		
	freeaddrinfo(servinfo);
//	printf("talker: sent %d bytes to %s\n", numbytes, argv[1]);
	printf("Phase 2: The File Request from Client 1 has been sent to the Directory Server\n");



	char t1[100],t2[100];
	sscanf(buf,"%s %s %s %s",t1,t2,phase3_server,phase3_port);
	printf("Phase 2: The File requested by Client 1 is present in File Server %s and the File Server's TCP port number is %s\n",phase3_server,phase3_port);
	
	printf("Phase 2: End of Phase 2 for Client 1\n");
	close(sockfd);
	return 0;
}

int phase3()
{
		
	int sockfd,status,sin_size,new_fd;
	char s[INET6_ADDRSTRLEN];
	struct addrinfo hints,* client,* p;
	struct sockaddr_in their_addr;	
	
	memset(&hints, 0, sizeof hints);
	
	hints.ai_family=AF_INET;
	hints.ai_socktype=SOCK_STREAM;
//	hints.ai_flags=AI_PASSIVE;
	
	if(status=getaddrinfo("127.0.0.1",phase3_port,&hints,&client)!=0)
	{
		printf("getaddrinfo: %s\n", gai_strerror(status));
		return 1;
	}
	
	
	for(p = client; p != NULL; p = p->ai_next) 
	{
		if ((sockfd = socket(p->ai_family, p->ai_socktype,p->ai_protocol)) == -1) 
		{
		perror("client: socket");
		continue;
		}
		
	
		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) 
		{
			close(sockfd);
			perror("client: connect");
		//	continue;
		}
		
	}
	
//	 print port
	int port;
	struct sockaddr_in sinport;
	socklen_t len=sizeof(sinport);
	if(getsockname(sockfd,(struct sockaddr *)&sinport,&len)==-1)
		perror("getsockname");
	else
		port=ntohs(sinport.sin_port);
	

//	end print port



//ip
/*
	void *addr;
	char f2_ip[INET6_ADDRSTRLEN];
	if (client->ai_family == AF_INET) 
	{ // IPv4
		struct sockaddr_in *ipv4 = (struct sockaddr_in *)client->ai_addr;
		addr = &(ipv4->sin_addr);
		
	}
	 else
	{ // IPv6
		struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)client->ai_addr;
		addr = &(ipv6->sin6_addr);
		
	}
	
	
	//ip 
	inet_ntop(client->ai_family,addr,f2_ip, sizeof f2_ip);
	
	printf("\n\n\nPhase 3: Client1 has dynamic TCP Port Number %d and IP address %s \n", port,f2_ip);
*/

struct hostent *he;
	if ((he=gethostbyname("nunki.usc.edu")) == NULL)
	{
		perror("gethostbyname");
		exit(1);
	}
	struct sockaddr_in ip_addr;
	ip_addr.sin_addr = *((struct in_addr *)he->h_addr);
	
	
	printf("\n\n\nPhase 3: Client 1 has dynamic TCP Port Number %d and IP address %s \n", port,inet_ntoa(ip_addr.sin_addr));
	
//end ip
	
	freeaddrinfo(client); // all done with this structure
	int numbytes;
	char buf[MAXDATASIZE];
//Send Doc1 to File Server
	if (send(sockfd, "Client1 doc1", 30, 0) == -1)
	{
		perror("send");
		exit(1);
	}
	else
	printf("Phase 3: The File request from Client 1 has been sent to File Server %s\n",phase3_server);

//Recevie Doc from File Server
	if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) 
	{
		perror("recv");
		exit(1);
	}
	else
	{
	buf[numbytes] = '\0';
	printf("Phase 3: Client 1 received %s from File Server %s\n",buf,phase3_server);
	}
	
	//printf("client: received %s\n",buf);
	printf("Phase 3: End of Phase3 for Client 1\n");
	close(sockfd);

return 0;
}

