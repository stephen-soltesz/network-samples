#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFFER_LEN  1000
#define PORT        21212
int main(int argc, char **argv)
{
    int       listen_fd=0;
    struct    sockaddr_in     server_addr;
    struct    sockaddr_in     client_addr;
    int       client_fd=0;
    int       ret=0;
    socklen_t client_addr_len;
    char      mesg_buffer[BUFFER_LEN];

    /* create a socket endpoint, not yet connected to anything */
    /* AF_INET - protocol family for IPv4, AF_INET6 for IPv6 */
    /* SOCK_STREAM - for stream protocol, like TCP */
    /* address family, 0 is unspecified */
    listen_fd = socket(AF_INET, SOCK_STREAM, 0);

    /* Initialize struct with server address. */
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    /*server_addr.sin_addr.s_addr=htonl(INADDR_ANY);*/  /* (u_int32_t)0x00000000 */
    ret = inet_pton(AF_INET, "192.168.1.117", &server_addr.sin_addr);
    if ( ret != 1 ) {
        fprintf(stderr, "Failed to convert inet_aton\n");
        exit(-1);
    }
    server_addr.sin_port=htons(PORT);
    fprintf(stderr, "addr: 0x%x\n", server_addr.sin_addr.s_addr);
    fprintf(stderr, "port: 0x%x\n", server_addr.sin_port);

    /* allow reusing of address before timewait */
    ret = 1;
    ret = setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &ret, sizeof(ret));
    if ( ret != 0 ) {
        perror("could not set REUSEADDR option on listen_fd");
        exit(errno);
    }

    /* bind the 'listen_fd' socket to the local server address & port */
    ret = bind(listen_fd,
               (struct sockaddr *)&server_addr,
               sizeof(server_addr));
    if ( ret != 0 ) {
        perror("failed to bind socket");
        exit(errno);
    }

    /* now set the socket to listen */
    ret = listen(listen_fd, 1);
    if ( ret != 0 ) {
        perror("could not listen!");
        exit(errno);
    }

    client_addr_len = sizeof(client_addr);
    while(1) {

        fprintf(stderr, "waiting for a new client on port %d..\n", PORT);
        client_fd = accept(listen_fd,
                           (struct sockaddr *)&client_addr,
                           &client_addr_len);

        /* check return value. */
        inet_ntop(AF_INET, &client_addr.sin_addr, mesg_buffer, INET_ADDRSTRLEN);
        fprintf(stderr, "client addr: %s\n", mesg_buffer);
        fprintf(stderr, "client port: %d\n", client_addr.sin_port);

        while(1) {
            ret = recvfrom(client_fd,
                         mesg_buffer,
                         BUFFER_LEN,
                         0,
                         (struct sockaddr *)&client_addr,
                         &client_addr_len);
            if ( ret < 0 ) {
                perror("recvfrom()");
                exit(errno);
            }
            if ( ret == 0 ) {
                /* client has disconnected */
                close(client_fd);
                break;
            }
            sendto(client_fd,
                   mesg_buffer,
                   ret,
                   0,
                   (struct sockaddr *)&client_addr,
                   sizeof(client_addr));
            mesg_buffer[ret] = '\0';
            printf("Received the following:\n");
            printf("%s", mesg_buffer);
            printf("-------------------------------------------------------\n");
        }
    }
}
