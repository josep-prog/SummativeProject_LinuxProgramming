#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <arpa/inet.h>


#define PORT 8080


// --- Reliable message framing (must match server.c) ---------------------
// A single send() is not guaranteed to arrive as a single recv() on the
// other end, and vice versa: TCP is a byte stream, not a message stream.
// Every text message is sent as a 4-byte length prefix (network byte
// order) followed by exactly that many payload bytes, and read back the
// same way, looping until the requested number of bytes has actually been
// transferred. This was necessary in practice: without it, two messages
// sent back-to-back by the server could coalesce into one recv() here
// (leaving a later recv() blocked forever), or a single message could
// arrive split across reads (silently dropping part of it).

ssize_t send_all(int sock, const void *buf, size_t len)
{
    size_t sent = 0;
    const char *p = buf;

    while(sent < len)
    {
        ssize_t n = send(sock, p + sent, len - sent, 0);

        if(n <= 0)
            return n;

        sent += (size_t)n;
    }

    return (ssize_t)sent;
}

ssize_t recv_all(int sock, void *buf, size_t len)
{
    size_t got = 0;
    char *p = buf;

    while(got < len)
    {
        ssize_t n = recv(sock, p + got, len - got, 0);

        if(n <= 0)
            return n;

        got += (size_t)n;
    }

    return (ssize_t)got;
}

int send_msg(int sock, const char *msg)
{
    uint32_t len = htonl((uint32_t)strlen(msg));

    if(send_all(sock, &len, sizeof(len)) <= 0)
        return -1;

    if(send_all(sock, msg, strlen(msg)) <= 0)
        return -1;

    return 0;
}

ssize_t recv_msg(int sock, char *buf, size_t buf_size)
{
    uint32_t len_net;

    ssize_t r = recv_all(sock, &len_net, sizeof(len_net));

    if(r <= 0)
        return r;

    uint32_t len = ntohl(len_net);

    if(len >= buf_size)
        len = (uint32_t)(buf_size - 1);

    r = recv_all(sock, buf, len);

    if(r <= 0)
        return r;

    buf[len] = '\0';

    return (ssize_t)len;
}



int main()
{

    int sock;


    sock =
    socket(AF_INET,
    SOCK_STREAM,
    0);

    if(sock < 0)
    {
        perror("socket() failed");
        exit(1);
    }



    struct sockaddr_in server;



    server.sin_family=AF_INET;

    server.sin_port=htons(PORT);


    inet_pton(AF_INET,
    "127.0.0.1",
    &server.sin_addr);



    if(connect(sock,
    (struct sockaddr*)&server,
    sizeof(server)) < 0)
    {
        perror("connect() failed");
        close(sock);
        exit(1);
    }



    int id;


    printf("Enter user ID: ");

    scanf("%d",&id);



    if(send_all(sock,&id,sizeof(id)) <= 0)
    {
        perror("send (user id) failed");
        close(sock);
        exit(1);
    }



    char buffer[500];

    ssize_t n;



    n = recv_msg(sock,buffer,sizeof(buffer));

    if(n <= 0)
    {
        printf("Server closed the connection.\n");
        close(sock);
        return 0;
    }



    printf("\n%s\n",
    buffer);



    if(strstr(buffer,"SUCCESS"))
    {

        int choice;


        printf("Choose equipment number: ");

        scanf("%d",&choice);



        if(send_all(sock,&choice,sizeof(choice)) <= 0)
        {
            perror("send (choice) failed");
            close(sock);
            exit(1);
        }



        n = recv_msg(sock,buffer,sizeof(buffer));

        if(n <= 0)
        {
            printf("Server closed the connection.\n");
            close(sock);
            return 0;
        }



        printf("%s\n",
        buffer);

    }



    printf("Session closed. Goodbye, %d\n",
    id);



    close(sock);



    return 0;

}
