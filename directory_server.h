

#define MYPORT "21676" // the port users will be connecting to
#define MYPORT_PHASE2 "31676" // the port users will be connecting to
#define MAXBUFLEN 100 // get sockaddr, IPv4 or IPv6:
#define FILE_SERVER1_TCP 41676
#define FILE_SERVER2_TCP 42676
#define FILE_SERVER3_TCP 43676

void *get_in_addr(struct sockaddr *);
void file_exsists();
int shortest_path(int,int,int,int);
int phase1();
int phase2();

