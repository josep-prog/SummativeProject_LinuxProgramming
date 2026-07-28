#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>


#define PORT 8080
#define MAX_CLIENTS 5


// --- Reliable message framing -------------------------------------------
// A single send() is not guaranteed to arrive as a single recv() on the
// other end: TCP is a byte stream, not a message stream. Two back-to-back
// sends can be coalesced into one recv(), and a single send() can just as
// easily be split across multiple recv() calls. Both were observed in
// practice while testing this server (a second recv() blocked forever on
// data that had already arrived in an earlier recv(); later, the tail of
// one message leaked into the read meant for the next). Every text message
// below is therefore sent as a 4-byte length prefix (network byte order)
// followed by exactly that many payload bytes, and read back the same way,
// looping until the requested number of bytes has actually been
// transferred in each direction.

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

// Sends msg as a 4-byte length prefix + payload. Returns 0 on success, -1
// on error/disconnection.

int send_msg(int sock, const char *msg)
{
    uint32_t len = htonl((uint32_t)strlen(msg));

    if(send_all(sock, &len, sizeof(len)) <= 0)
        return -1;

    if(send_all(sock, msg, strlen(msg)) <= 0)
        return -1;

    return 0;
}

// Reads a length-prefixed message into buf (which must be at least
// buf_size bytes). Returns the payload length on success, 0 if the peer
// disconnected cleanly, or -1 on error. buf is always null-terminated on
// a successful (>0) return.

ssize_t recv_msg(int sock, char *buf, size_t buf_size)
{
    uint32_t len_net;

    ssize_t r = recv_all(sock, &len_net, sizeof(len_net));

    if(r <= 0)
        return r;

    uint32_t len = ntohl(len_net);

    if(len >= buf_size)
        len = (uint32_t)(buf_size - 1);   // defensive truncation

    r = recv_all(sock, buf, len);

    if(r <= 0)
        return r;

    buf[len] = '\0';

    return (ssize_t)len;
}


// Shared equipment data

char *equipment[] = {
    "Laptop",
    "Projector",
    "Camera",
    "VR Headset"
};


int reserved[4] = {0,0,0,0};


// Mutex protects reservations and connected_users

pthread_mutex_t lock;


// Valid users

int users[] = {
    101,
    102,
    103,
    104,
    105
};


// Currently connected (authenticated) users, -1 = empty slot

int connected_users[MAX_CLIENTS];



int authenticate(int id)
{
    for(int i=0;i<5;i++)
    {
        if(users[i] == id)
            return 1;
    }

    return 0;
}



// Add user_id to the connected_users table. Returns 1 on success,
// 0 if there was no free slot.

int add_connected_user(int user_id)
{
    int added = 0;

    pthread_mutex_lock(&lock);

    for(int i=0;i<MAX_CLIENTS;i++)
    {
        if(connected_users[i] == -1)
        {
            connected_users[i] = user_id;
            added = 1;
            break;
        }
    }

    pthread_mutex_unlock(&lock);

    return added;
}



// Remove user_id from the connected_users table (no-op if not present).

void remove_connected_user(int user_id)
{
    pthread_mutex_lock(&lock);

    for(int i=0;i<MAX_CLIENTS;i++)
    {
        if(connected_users[i] == user_id)
        {
            connected_users[i] = -1;
            break;
        }
    }

    pthread_mutex_unlock(&lock);
}



// Display currently connected users and equipment reservation status.

void print_status()
{
    pthread_mutex_lock(&lock);

    printf("\n--- Server Status ---\n");

    printf("Connected users: ");

    int any = 0;

    for(int i=0;i<MAX_CLIENTS;i++)
    {
        if(connected_users[i] != -1)
        {
            printf("%d ", connected_users[i]);
            any = 1;
        }
    }

    if(!any)
        printf("(none)");

    printf("\n");

    printf("Equipment status:\n");

    for(int i=0;i<4;i++)
    {
        printf("  %d. %s: %s\n",
               i+1,
               equipment[i],
               reserved[i] ? "Reserved" : "Available");
    }

    printf("---------------------\n\n");

    pthread_mutex_unlock(&lock);
}



// Appends the equipment listing text onto the end of an existing buffer.
// This does NOT send anything itself: the caller combines it with any other
// text (e.g. "LOGIN SUCCESS") into a single message so that exactly one
// send() on the server side matches exactly one recv() on the client side.
// Sending "LOGIN SUCCESS" and the equipment list as two separate writes
// let TCP coalesce them into a single recv() on the client, which then
// blocked forever on a second recv() waiting for data that had already
// arrived — this was confirmed by an actual client/server run that hung.

void append_equipment_list(char *message)
{
    pthread_mutex_lock(&lock);

    for(int i=0;i<4;i++)
    {
        char temp[100];

        sprintf(temp,
        "%d. %s (%s)\n",
        i+1,
        equipment[i],
        reserved[i] ? "Reserved":"Available");


        strcat(message,temp);
    }

    pthread_mutex_unlock(&lock);
}



void *client_handler(void *arg)
{

    int client = *(int *)arg;

    free(arg);


    int user_id;
    int authenticated_user_added = 0;


    ssize_t n = recv_all(client,&user_id,sizeof(user_id));

    if(n <= 0)
    {
        // Client disconnected or a socket error occurred before sending an ID.
        printf("Client disconnected before authentication\n");
        close(client);
        return NULL;
    }


    if(authenticate(user_id))
    {
        if(add_connected_user(user_id))
        {
            authenticated_user_added = 1;
        }

        print_status();

        char welcome[600];

        strcpy(welcome, "LOGIN SUCCESS\nAvailable Equipment:\n");

        append_equipment_list(welcome);

        if(send_msg(client,welcome) < 0)
        {
            perror("send (login success + equipment) failed");
        }


        int choice;

        n = recv_all(client,&choice,sizeof(choice));

        if(n <= 0)
        {
            // Client disconnected before choosing equipment.
            printf("User %d disconnected before making a choice\n", user_id);
        }
        else
        {

        choice--;


        if(choice < 0 || choice >= 4)
        {

            char msg[]=
            "Invalid equipment choice";


            if(send_msg(client,msg) < 0)
            {
                perror("send (invalid choice) failed");
            }

        }

        else
        {

        pthread_mutex_lock(&lock);



        if(reserved[choice]==0)
        {

            reserved[choice]=1;


            char msg[100];

            sprintf(msg,
            "Reservation successful: %s",
            equipment[choice]);


            if(send_msg(client,msg) < 0)
            {
                perror("send (reservation success) failed");
            }

        }


        else
        {

            char msg[]=
            "Equipment already reserved";


            if(send_msg(client,msg) < 0)
            {
                perror("send (reservation rejected) failed");
            }

        }



        pthread_mutex_unlock(&lock);

        }

        print_status();

        }

    }


    else
    {

        char fail[]="LOGIN FAILED";

        if(send_msg(client,fail) < 0)
        {
            perror("send (login failed) failed");
        }

    }


    if(authenticated_user_added)
    {
        remove_connected_user(user_id);
        print_status();
    }


    printf("User disconnected\n");


    close(client);


    return NULL;

}



void init_connected_users()
{
    for(int i=0;i<MAX_CLIENTS;i++)
    {
        connected_users[i] = -1;
    }
}



int main()
{

    int server_socket;


    server_socket =
    socket(AF_INET,
           SOCK_STREAM,
           0);

    if(server_socket < 0)
    {
        perror("socket() failed");
        exit(1);
    }



    struct sockaddr_in server;


    server.sin_family=AF_INET;

    server.sin_port=htons(PORT);

    server.sin_addr.s_addr=INADDR_ANY;



    if(bind(server_socket,
        (struct sockaddr*)&server,
        sizeof(server)) < 0)
    {
        perror("bind() failed");
        close(server_socket);
        exit(1);
    }



    if(listen(server_socket,MAX_CLIENTS) < 0)
    {
        perror("listen() failed");
        close(server_socket);
        exit(1);
    }



    pthread_mutex_init(&lock,NULL);

    init_connected_users();



    printf("Server running on port %d\n",
    PORT);



    while(1)
    {

        int client;


        client =
        accept(server_socket,NULL,NULL);

        if(client < 0)
        {
            // A single failed accept() should not bring the server down;
            // log it and keep serving other clients.
            perror("accept() failed");
            continue;
        }



        printf("Client connected\n");



        pthread_t thread;



        int *p =
        malloc(sizeof(int));

        if(p == NULL)
        {
            fprintf(stderr, "malloc() failed, dropping connection\n");
            close(client);
            continue;
        }

        *p=client;



        if(pthread_create(
            &thread,
            NULL,
            client_handler,
            p
        ) != 0)
        {
            perror("pthread_create() failed");
            free(p);
            close(client);
            continue;
        }



        pthread_detach(thread);

    }


    return 0;
}
