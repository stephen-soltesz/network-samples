#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFFER_LEN  256
#define PORT        21212
int main(int argc, char *argv[])
{
    struct sockaddr_in     server_addr;
    struct in_addr         addr;
    int       client_fd=0;
    int       ret;
    char      mesg_buffer[BUFFER_LEN];

    if (argc < 2) {
        fprintf(stderr,"usage %s hostname\n", argv[0]);
        exit(0);
    }

    /* create a socket end point */
    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

    bzero(&addr, sizeof(addr));
    ret = inet_pton(AF_INET, argv[1], &addr);
    if ( ret != 1 ) {
        fprintf(stderr, "Failed to convert inet_aton(AF_INET, %s)\n", argv[1]);
        exit(-1);
    }

    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = addr.s_addr;
    server_addr.sin_port = htons(PORT);

    /* Now connect to the server */
    ret = connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (ret < 0) {
         perror("ERROR connecting");
         exit(errno);
    }

    fprintf(stderr, "Enter a message: ");
    bzero(mesg_buffer,BUFFER_LEN);
    fgets(mesg_buffer,BUFFER_LEN-1,stdin);

    /* Send message to the server */
    ret = sendto(client_fd,
                 mesg_buffer,
                 strlen(mesg_buffer),
                 0,
                 NULL, 0);
                 /*(struct sockaddr *)&server_addr,
                 sizeof(server_addr));*/
    if (ret < 0) {
         perror("ERROR writing to socket");
         exit(1);
    }

    /* Now read server response */
    bzero(mesg_buffer,BUFFER_LEN);
    ret = recvfrom(client_fd,
                   mesg_buffer,
                   BUFFER_LEN-1,
                   0,
                   NULL, NULL);
                   /*(struct sockaddr *)&server_addr,
                   sizeof(server_addr));*/
    if (ret < 0) {
         perror("ERROR reading from socket");
         exit(1);
    }
    fprintf(stderr, "Received: %s\n", mesg_buffer);
    return 0;
}
