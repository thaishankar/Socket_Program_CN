#! /bin/bash


gcc -g -o directory_server directory_server.c -lnsl -lsocket -lresolv 
gcc -g -o file_server1 file_server1.c -lnsl -lsocket -lresolv 
gcc -g -o file_server2 file_server2.c -lnsl -lsocket -lresolv  
gcc -g -o file_server3 file_server3.c -lnsl -lsocket -lresolv 
gcc -g -o client1 client1.c -lnsl -lsocket -lresolv 
gcc -g -o client2 client2.c -lnsl -lsocket -lresolv 

#./directory_server &
#./file_server1 localhost
#./file_server2 localhost
#./file_server3 localhost
#./file_server1 localhost
#./file_server2 localhost
#./file_server2 localhost
#./client1 localhost
#./client2 localhost
