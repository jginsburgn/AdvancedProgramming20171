/*
 Simple program for a server using sockets
 Can only connect one client at a time
 */

#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Include libraries for sockets


#define MAX_CONNECTIONS 5
#define BUFFER_SIZE 1024

void usage(char * program);
void startServer(char * port);
void waitForConnections(int server_fd);
void communicationLoop(int connection_fd);

int main(int argc, char * argv[])
{
    printf("=== SERVER PROGRAM ===\n");
    
    if (argc != 2)
        usage(argv[0]);
    
    startServer(argv[1]);
    
    return 0;
}

// Show the user how to run this program
void usage(char * program)
{
    printf("Usage:\n%s {port_number}\n", program);
    exit(EXIT_FAILURE);
}

// Initialize the server to be ready for connections
void startServer(char * port)
{
    int socket_descriptor = socket(PF_INET, SOCK_STREAM, 0);
    if (socket_descriptor == -1) {
        printf("");
    }
    
    
}

// Stand by for connections by the clients
void waitForConnections(int server_fd)
{
    
}

// Talk between client and server
void communicationLoop(int client_fd)
{
    
}
