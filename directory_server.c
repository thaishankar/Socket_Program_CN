#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>

#include <errno.h>

#include "directory_server.h"	
  
char fs1[50],fs2[50],fs3[50],temp_serv[50];
int port_f1,port_f2,port_f3;

int shortest_path(int f1,int f2,int f3,int client_id)
{
//	int FILE_SERVER1_TCP=41676,FILE_SERVER2_TCP=42676,FILE_SERVER3_TCP=43676;
	char path_f1[10],path_f2[10],path_f3[10];
	
	int dist_f1,dist_f2,dist_f3;
	char line2[50],temp[50];
	FILE *file2,*file3;
	char *r;
	int k;
	file2=fopen("topology.txt", "r");		

	file3=fopen("directory.txt","r");
	
// Read File Server and Port number from directory.txt
	for(k=0;k<3;k++)
	{
		r=fgets(line2,50,file3);
		if(strstr(line2,"File_Server1")!=NULL)
		{
			sscanf(line2,"%s %d",fs1,&port_f1);
			sprintf(fs1,"%s","File Server 1");
		}
		else if(strstr(line2,"File_Server2")!=NULL)
		{
			sscanf(line2,"%s %d",fs2,&port_f2);		
			sprintf(fs2,"%s","File Server 2");
		}
		else if(strstr(line2,"File_Server3")!=NULL)
		{
			sscanf(line2,"%s %d",fs3,&port_f3);		
			sprintf(fs3,"%s","File Server 3");
		}
	}
	
	
	fclose(file3);		
//	printf("Distances of 1 is %d 2 is %d 3 is %d\n",port_f1,port_f2,port_f3);

	if(client_id==1)
		r=fgets(line2,50,file2);
	else if(client_id==2)
		for(k=0;k<2;k++)
			r=fgets(line2,50,file2);

//Convert Distances to integer from String
	if(r!=NULL)
		sscanf(line2,"%s %s %s",path_f1,path_f2,path_f3);
	
	dist_f1=atoi(path_f1);	
	dist_f2=atoi(path_f2);
	dist_f3=atoi(path_f3);

// Check which File_Server the doc is present in and return the Port number of the File_Server with the lowest cost	
		
	if(f1==1 && f2==0 && f3==0)
		return dist_f1;
	else if(f1==0 && f2==1 && f3==0)
		return dist_f2;
	else if(f1==0 && f2==0 && f3==1)
		return dist_f3;

	else if(f1==1 && f2==1 && f3==0)
	{	
		if(dist_f1>=dist_f2)
			return port_f2;
		else 
			return port_f1;	
	}

	else if(f1==0 && f2==1 && f3==1)
	{	
		if(dist_f2>=dist_f3)
			return port_f3;
		else 
			return port_f2;	
	}
	
	else if(f1==1 && f2==0 && f3==1)
	{	
		if(dist_f1>=dist_f3)
			return port_f3;
		else 
			return port_f1;	
	}
	
	else if(f1==1 && f2==1 && f3==1)
	{	
		if((dist_f1<=dist_f2)&&(dist_f1<=dist_f3))
			return port_f1;
		else if((dist_f2<=dist_f1)&&(dist_f2<=dist_f3))
			return port_f2;	
		else 
			return port_f3;
	}

fclose(file2);

}
	
//Function to return addr field
void *get_in_addr(struct sockaddr *sa)
{
if (sa->sa_family == AF_INET) {
return &(((struct sockaddr_in*)sa)->sin_addr);
}
return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(void)
{

	file_exsists();
	int return_val=phase1();
	printf("***************************************************************\n");	
	return_val=phase2();
	return 0;
}



int phase1()
{

	int yes=1;
	int sockfd;
	struct addrinfo hints, *dir_server, *p;
	int rv;
	int numbytes;
	struct sockaddr_storage their_addr;
	char buf[MAXBUFLEN];
	socklen_t addr_len;
	char s[INET6_ADDRSTRLEN];
	char dir_ip[INET6_ADDRSTRLEN];

	memset(&hints, 0, sizeof hints);
	
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
//	hints.ai_flags = AI_PASSIVE;
	
	if ((rv = getaddrinfo("localhost", MYPORT, &hints, &dir_server)) != 0) 
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return -1;
	}
	

	
// Loop through all the results and bind to a socket for Phase1
	for(p = dir_server; p != NULL; p = p->ai_next) 
	{
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
p->ai_protocol)) == -1) 
		{
			perror("listener: socket");
			continue;
		}

// remove any exsisting bind
		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
sizeof(int)) == -1) 
		{
			perror("setsockopt");
			return(-1);
		}


		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) 
		{
			close(sockfd);
			perror("listener: bind");
			continue;
		}
		break;
	}
	
	
	if (p == NULL) 
	{
		fprintf(stderr, "listener: failed to bind socket\n");
		return -1;
	}
	
	freeaddrinfo(dir_server);


	int port=0;


/*
	void *addr;
	if (p->ai_family == AF_INET) 
	{ // IPv4
		struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
		addr = &(ipv4->sin_addr);
	}
	 else
	{ // IPv6
		struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
		addr = &(ipv6->sin6_addr);
	}
	
	
	//ip 
	inet_ntop(p->ai_family,addr,dir_ip, sizeof dir_ip);
	
		printf("\n\nPhase 1: The Directory Server has UDP Port Number %d and IP address %s \n", port,dir_ip);

*/	
	//Get port number
	struct sockaddr_in sinport;
	socklen_t len=sizeof(sinport);
	if(getsockname(sockfd,(struct sockaddr *)&sinport,&len)==-1)
		perror("getsockname");
	else
		port=ntohs(sinport.sin_port);

	//end port
	
	
	//Get IP address
	struct hostent *he;
	if ((he=gethostbyname("nunki.usc.edu")) == NULL)
	{
		perror("gethostbyname");
		exit(1);
	}
	struct sockaddr_in ip_addr;
	ip_addr.sin_addr = *((struct in_addr *)he->h_addr);
	
	
	printf("\n\nPhase 1: The Directory Server has UDP Port Number %d and IP address %s \n", port,inet_ntoa(ip_addr.sin_addr));

	addr_len = sizeof their_addr;

int count =0;

while(count<=2)
{

	//Receive Port number from the 3 File_Servers
	if ((numbytes = recvfrom(sockfd, buf, MAXBUFLEN-1 , 0,
(struct sockaddr *)&their_addr, &addr_len)) == -1) 
	{	
		perror("recvfrom");
		return(-1);
	}
	
	if(numbytes > 0)
		count++;
	

	//printf("listener: got packet from %s\n",
	inet_ntop(their_addr.ss_family,get_in_addr((struct sockaddr *)&their_addr),s, sizeof s);
	//printf("listener: packet is %d bytes long\n", numbytes);
	buf[numbytes] = '\0';
	//printf("listener: packet contains \"%s\"\n", buf);

	FILE *file;
	file=fopen("directory.txt", "a");
//	fprintf(file,count);
	fprintf(file,buf);
	fclose(file);
	

	printf("Phase 1: The Directory Server has received request from File Server %c\n",buf[11]);	
		
		
}	
	
	
	printf("\n\nPhase 1: The directory.txt file has been created. \n\nEnd of Phase1 for the Directory Server\n");
	close(sockfd);	
	return 0;
}

//Create new file directory.txt at the start of the program
void file_exsists()
{
	FILE *file;
	file=fopen("directory.txt", "w");
  	fclose(file);
  
 }
	 



int phase2()
{
	
	int yes=1;
	int sockfd;
	struct addrinfo hints, *dir_server, *p;
	int rv;
	int numbytes;
	struct sockaddr_storage their_addr;
	char buf[MAXBUFLEN];
	socklen_t addr_len;
	char s[INET6_ADDRSTRLEN];
	char dir_ip[INET6_ADDRSTRLEN];

	memset(&hints, 0, sizeof hints);
	
	hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
	hints.ai_socktype = SOCK_DGRAM;
//	hints.ai_flags = AI_PASSIVE; // use my IP
	
	if ((rv = getaddrinfo("localhost", MYPORT_PHASE2, &hints, &dir_server)) != 0) 
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return -1;
	}
	

	
// loop through all the results and bind to the first we can
	for(p = dir_server; p != NULL; p = p->ai_next) 
	{
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
p->ai_protocol)) == -1) 
		{
			perror("listener: socket");
			continue;
		}

// remove any exsisting bind
		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
sizeof(int)) == -1) 
		{
			perror("setsockopt");
			return(-1);
		}


		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) 
		{
			close(sockfd);
			perror("listener: bind");
			continue;
		}
		break;
	}
	
	if (p == NULL) 
	{
		fprintf(stderr, "listener: failed to bind socket\n");
		return -1;
	}
	
	freeaddrinfo(dir_server);
	int port=0;
/*
	void *addr;
	if (p->ai_family == AF_INET) 
	{ // IPv4
		struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
		addr = &(ipv4->sin_addr);
	}
	 else
	{ // IPv6
		struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
		addr = &(ipv6->sin6_addr);
	}
	
	
	//ip 
	inet_ntop(p->ai_family,addr,dir_ip, sizeof dir_ip);
	
		printf("\n\nPhase 2: The Directory Server has UDP Port Number %d and IP address %s \n", port,dir_ip);

*/	
	//Get port Number
	struct sockaddr_in sinport;
	socklen_t len=sizeof(sinport);
	if(getsockname(sockfd,(struct sockaddr *)&sinport,&len)==-1)
		perror("getsockname");
	else
		port=ntohs(sinport.sin_port);

	//end port
	
	
	//Get IP Address
	struct hostent *he;
	if ((he=gethostbyname("nunki.usc.edu")) == NULL)
	{
		perror("gethostbyname");
		exit(1);
	}
	struct sockaddr_in ip_addr;
	ip_addr.sin_addr = *((struct in_addr *)he->h_addr);
	
	
	printf("\n\nPhase 2: The Directory Server has UDP Port Number %d and IP address %s \n", port,inet_ntoa(ip_addr.sin_addr));
	
	
	addr_len = sizeof their_addr;

int count=0;

while(count<2)
{

	//Receive 2 requests from Clients
	if ((numbytes = recvfrom(sockfd, buf, MAXBUFLEN-1 , 0,
(struct sockaddr *)&their_addr, &addr_len)) == -1) 
	{
		perror("recvfrom");
		return(-1);
	}
	
	if(numbytes > 0)
		count++;
	

	//printf("listener: got packet from %s\n",
	inet_ntop(their_addr.ss_family,get_in_addr((struct sockaddr *)&their_addr),s, sizeof s);
	//printf("listener: packet is %d bytes long\n", numbytes);
	buf[numbytes] = '\0';
	//printf("listener: packet contains \"%s\"\n", buf);

	printf("Phase 2: The Directory Server has received request from Client %c\n",buf[6]);	

// chk in which server file exsists
	//printf("no fault 1\n");
	char line[50];
	char *r;
		
	FILE *file;
	file=fopen("resource.txt", "r");	
	int f1=0,f2=0,f3=0;
	int i,j;
	int c_id=0;
	int dist;
			
	for(i=0;i<3;i++)
	{
		r=fgets(line,50,file);
		//sscanf(line,"%d/t%d/t%d",dist_f1,dist_f2,dist_f3);
		
		if(r != NULL)
		{
			if(buf[6]=='1'&&buf[11]=='1')
			{
				char file_search[]="doc1";
				c_id=1;
				if(strstr(line,file_search)!=NULL)
				{
			
		//			printf("File found on File_Server%d\n",(i+1));
					if((i+1)==1)
						f1=1;
					else if((i+1)==2)
						f2=1;
					else if((i+1)==3)
						f3=1;
				}
			}
			else if(buf[6]=='2'&&buf[11]=='2')
			{
				char file_search[]="doc2";
				c_id=2;
				if(strstr(line,file_search)!=NULL)
				{
		//			printf("File found on File_Server%d\n",(i+1));
					if((i+1)==1)
						f1=1;
					else if((i+1)==2)
						f2=1;
					else if((i+1)==3)
						f3=1;
				}
			}
		}
	}

	dist=shortest_path(f1,f2,f3,c_id);
	
//Send File_Server name and corresponding Port number to the Clients
//	printf("Shortest distance = %d\n",dist);
	char chk[20];
	char *send_to_client;
	sprintf(chk,"%d",dist);

	if(dist==port_f1)
	{
		send_to_client=strcat(fs1," ");
		send_to_client=strcat(send_to_client,chk);
	}
	else if(dist==port_f2)
	{
		send_to_client=strcat(fs2," ");
		send_to_client=strcat(send_to_client,chk);
	}
	else if(dist==port_f3)
	{
		send_to_client=strcat(fs3," ");
		send_to_client=strcat(send_to_client,chk);
	}
		
	if ((numbytes = sendto(sockfd, send_to_client , strlen(send_to_client), 0,(struct sockaddr *)&their_addr, addr_len)) == -1) 
	{
		perror("talker: sendto");
		exit(1);
	}
	
	printf("Phase 2: File Server details has been sent to Client %d \n",c_id);
	
	fclose(file);
	f1=0;f2=0;f3=0;
	
}
	
	
	printf("\nPhase 2: End of phase2 for the Directory Server\n");
	close(sockfd);	
	return 0;


}//end phase2
