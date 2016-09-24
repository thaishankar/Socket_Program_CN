
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

#include "file_server.h"

void *get_in_addr(struct sockaddr *sa)
{
if (sa->sa_family == AF_INET) {
return &(((struct sockaddr_in*)sa)->sin_addr);
}
return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


int main()
{
	int return_val=phase1();
	printf("***************************************************************\n");
	return_val=phase3();
	return 0;
}

int phase1()
{
	usleep(20000);
	int sockfd,sockfd2;
	struct addrinfo hints, *servinfo, *p, *client, *p2;
	int rv;
	int numbytes;
	int yes=1;
	char send_to_dir[30]="File_Server1 41676\n";
	int len_send_to_dir=strlen(send_to_dir);


		
/*	if (num_arg != 2) 
	{
		fprintf(stderr,"usage: talker hostname message\n");
		exit(1);
	}
*/	
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
//	hints.ai_flags = AI_PASSIVE;
	if ((rv = getaddrinfo("localhost", SERVERPORT, &hints, &servinfo)) != 0) 
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}
	
	
	// start bind

	if ((rv = getaddrinfo("localhost", FILE_SERVER1, &hints, &client)) != 0) 
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
	
	printf("\nPhase 1: File Server1 has UDP Port Number %d and IP address %s \n", port,f2_ip);
*/
//Print IP
struct hostent *he;
	if ((he=gethostbyname("nunki.usc.edu")) == NULL)
	{
		perror("gethostbyname");
		exit(1);
	}
	struct sockaddr_in ip_addr;
	ip_addr.sin_addr = *((struct in_addr *)he->h_addr);
	
	
	printf("\nPhase 1: File Server 1 has UDP Port Number %d and IP address %s \n", port,inet_ntoa(ip_addr.sin_addr));
	
	
//end ip





// loop through all the results and make a socket
//	for(p = servinfo; p != NULL; p = p->ai_next) 
//	{
	/*	if ((sockfd = socket(p->ai_family, p->ai_socktype,
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
	}*/
	
	//Sent Port number to Directory Server
	p = servinfo;
	if ((numbytes = sendto(sockfd2, send_to_dir , len_send_to_dir, 0,
p->ai_addr, p->ai_addrlen)) == -1) 
	{
		perror("talker: sendto");
		exit(1);
	}
	
	freeaddrinfo(servinfo);
//	printf("talker: sent %d bytes to %s\n", numbytes, argv[1]);
	printf("Phase 1: The Registration request from File Server 1 has been sent to the Directory Server\n");
	printf("Phase 1: End of Phase 1 for File Server 1\n");
	close(sockfd);
	return 0;
}

int phase3()
{
	int sockfd,status,sin_size,new_fd;
	
	struct addrinfo hints,* serv,* p;
	struct sockaddr_in their_addr;	
	
	memset(&hints, 0, sizeof hints);
	
//	hints.ai_flags=AI_PASSIVE;
	hints.ai_family=AF_INET;
	hints.ai_socktype=SOCK_STREAM;
	
	if(status=getaddrinfo("localhost",TCP_FILE_SERVER1,&hints,&serv)!=0)
	{
		printf("getaddrinfo: %s\n", gai_strerror(status));
		return 1;
	}
	
	
	for(p = serv; p != NULL; p = p->ai_next) 
	{
		if ((sockfd = socket(p->ai_family, p->ai_socktype,p->ai_protocol)) == -1) 
		{
		perror("server: socket");
		continue;
		}
	}
	
		
	if (bind(sockfd, serv->ai_addr, serv->ai_addrlen) == -1) 
	{
		close(sockfd);
		perror("server: bind");
		return 1;
	}
	
	freeaddrinfo(serv); // all done with this structure
	if (listen(sockfd, BACKLOG) == -1) 
	{
		perror("listen");
		exit(1);
	}


//phase3 ip and port

int port;
	struct sockaddr_in sinport;
	socklen_t len=sizeof(sinport);
	if(getsockname(sockfd,(struct sockaddr *)&sinport,&len)==-1)
		perror("getsockname");
	else
		port=ntohs(sinport.sin_port);


/*	
	void *addr;
	char f2_ip[INET6_ADDRSTRLEN];
	if (serv->ai_family == AF_INET) 
	{ // IPv4
		struct sockaddr_in *ipv4 = (struct sockaddr_in *)serv->ai_addr;
		addr = &(ipv4->sin_addr);
		
	}
	 else
	{ // IPv6
		struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)serv->ai_addr;
		addr = &(ipv6->sin6_addr);
		
	}
	
	
	//ip 
	inet_ntop(serv->ai_family,addr,f2_ip, sizeof f2_ip);
	
		printf("\nPhase 3: File Server1 has TCP Port Number %d and IP address %s \n", port,f2_ip);

*/
//Print IP and Port number
struct hostent *he;
	if ((he=gethostbyname("nunki.usc.edu")) == NULL)
	{
		perror("gethostbyname");
		exit(1);
	}
	struct sockaddr_in ip_addr;
	ip_addr.sin_addr = *((struct in_addr *)he->h_addr);
	
	
	printf("\nPhase 3: File Server 1 has TCP Port Number %d and IP address %s \n", port,inet_ntoa(ip_addr.sin_addr));
	
	
//end phase 3 ip and port

int flag;
	
	while(1) 
	{ // main accept() loop
	flag=0;
		sin_size = sizeof their_addr;
		char s[INET6_ADDRSTRLEN];
		//Receive doc requests from Client
		new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
		inet_ntop(their_addr.sin_family,&(their_addr.sin_addr),s,sizeof s);
		
//		printf("server: got connection from %s \n",s);
		
		int numbytes;
		char buf[100];
		
		if (new_fd == -1) 
		{
			perror("accept");
			return 1;
		}
		else
		{
			if (!fork()) 
			{ // this is the child process
				close(sockfd); // child doesn't need the listener
//Receive Doc from Client
				if ((numbytes = recv(new_fd, buf, 99, 0)) == -1) 
				{
					perror("recv");
					exit(1);
				}
				else
				flag=1;
				
				buf[numbytes] = '\0';
	//			printf("server: received %s\n",buf);

				char client_req_id[30],doc_id[30];
				sscanf(buf,"%s %s",client_req_id,doc_id);
				
				if(flag==1)
				printf("Phase 3: File Server 1 received the request from the %s for the file %s\n",client_req_id,doc_id);
				
//Send doc to the Client
				if (send(new_fd, doc_id, 30, 0) == -1)
				perror("send");
				else
				printf("Phase 3: File Server 1 has sent %s to %s\n",doc_id,client_req_id);
				
//				break;			
				close(new_fd);
				exit(0);
			}//close fork
		}// close else
//	count++;
//break;
	}

//printf("Phase 3: End of Phase 3 for File Server 1\n");
// close while
	close(new_fd); // parent doesn't need this
	
return 0;
}
