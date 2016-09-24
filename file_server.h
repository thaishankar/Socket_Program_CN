#define SERVERPORT "21676"
#define FILE_SERVER1 "22676"
#define FILE_SERVER2 "23676"
#define FILE_SERVER3 "24676"

#define TCP_FILE_SERVER1 "41676"
#define TCP_FILE_SERVER2 "42676"
#define TCP_FILE_SERVER3 "43676"
#define MAXDATASIZE 100
#define BACKLOG 10

void *get_in_addr(struct sockaddr *);
int phase1();
int phase3();

