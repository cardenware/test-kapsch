#include "networking.h"


/**
 * Hosts a local server on the specified port.
 *
 * @param port: the port to host the server on.
 *
 *@return:  the file descriptor of the socket if successful.
 *          -1 if socket creation fails.
 *          -2 if binding fails.
 *          -3 if listening fails.
 */
int hostLocalServer(int port)
{
    int socketFd;
    struct sockaddr_in serverAddr;

    socketFd = socket(AF_INET, SOCK_STREAM, 0);

    if (socketFd == -1)
    {
        printf("Socket creation failed\n");
        return -1;
    }

    printf("Socket created!\n");

    memset(&serverAddr, 0, sizeof(serverAddr));

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(port);

    if ((bind(socketFd, (struct sockaddr *)&serverAddr, sizeof(serverAddr))) != 0)
    {
        printf("Socketed bind failed\n");
        return -2;
    }

    printf("Socket binded!\n");

    if ((listen(socketFd, 5)) != 0)
    {
        printf("Listen failed\n");
        return -3;
    }

    printf("Socket listening on port %d...\n", port);

    return socketFd;
}


/**
 * Connects to a server at the specified host and port.
 *
 * @param host: A string representing the host IP address or hostname.
 * @param port: An integer representing the port number.
 *
 * @return: the socket file descriptor if successful.
 *          -1 if socket creation fails.
 *          -2 if the given address or address type is not supported.
 *          -3 if connection fails.
 */
int connect2server(char *host, int port)
{
    int socketFd, ServerAddrLen;
    struct sockaddr_in serverAddr, ServerAddr;

    ServerAddrLen = sizeof(ServerAddr);

    socketFd = socket(AF_INET, SOCK_STREAM, 0);

    if (socketFd == -1)
    {
        printf("Socket creation failed...\n");
        return -1;
    }

    printf("Socket created!\n");

    memset(&serverAddr, 0, sizeof(serverAddr));

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);

    if (inet_pton(AF_INET, host, &serverAddr.sin_addr) <= 0) {
        printf("Invalid address or address not supported...\n");
        return -2;
    }

    if ((connect(socketFd, (struct sockaddr *)&serverAddr, ServerAddrLen)) < 0)
    {
        printf("Connection failed...\n");
        return -3;
    }
    
    printf("Connected to the server %s @ %d...\n", host, port);

    return socketFd;
}
