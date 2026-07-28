#include <stdio.h>
#include <pthread.h>
#include <unistd.h>


#define SIZE 5
#define ORDERS 20


// Order structure
typedef struct {
    int id;
} Order;


// Shared queue
Order queue[SIZE];

int front = 0;
int rear = 0;
int count = 0;

int prepared = 0;
int delivered = 0;


// Synchronization tools
pthread_mutex_t mutex;

pthread_cond_t not_full;
pthread_cond_t not_empty;



// Add order to queue
void add_order(Order order)
{
    queue[rear] = order;

    rear = (rear + 1) % SIZE;

    count++;
}



// Remove order from queue
Order remove_order()
{
    Order order = queue[front];

    front = (front + 1) % SIZE;

    count--;

    return order;
}



// Kitchen thread
void *kitchen(void *arg)
{

    for(int i = 1; i <= ORDERS; i++)
    {

        sleep(2);   // preparing food


        Order order;

        order.id = i;



        pthread_mutex_lock(&mutex);



        // Wait if queue is full
        while(count == SIZE)
        {
            pthread_cond_wait(&not_full,&mutex);
        }



        add_order(order);

        prepared++;


        printf("Kitchen prepared order %d\n",i);



        // Tell delivery thread
        pthread_cond_signal(&not_empty);



        pthread_mutex_unlock(&mutex);

    }


    return NULL;
}



// Delivery thread
void *delivery(void *arg)
{

    for(int i = 0; i < ORDERS; i++)
    {


        pthread_mutex_lock(&mutex);



        // Wait if queue empty
        while(count == 0)
        {
            pthread_cond_wait(&not_empty,&mutex);
        }



        Order order = remove_order();



        delivered++;


        // Tell kitchen space exists
        pthread_cond_signal(&not_full);



        pthread_mutex_unlock(&mutex);



        sleep(4);


        printf("Delivered order %d\n",
               order.id);

    }


    return NULL;
}




// Monitoring thread
void *monitor(void *arg)
{

    int done = 0;

    while(!done)
    {

        sleep(5);


        pthread_mutex_lock(&mutex);



        printf("\n--- Monitor ---\n");

        printf("Orders prepared: %d\n",
               prepared);


        printf("Orders delivered: %d\n",
               delivered);


        printf("Queue size: %d\n",
               count);


        printf("----------------\n\n");


        done = (delivered >= ORDERS);


        pthread_mutex_unlock(&mutex);

    }


    return NULL;
}




int main()
{

    pthread_t kitchen_thread;
    pthread_t delivery_thread;
    pthread_t monitor_thread;



    pthread_mutex_init(&mutex,NULL);

    pthread_cond_init(&not_full,NULL);

    pthread_cond_init(&not_empty,NULL);



    pthread_create(
        &kitchen_thread,
        NULL,
        kitchen,
        NULL
    );


    pthread_create(
        &delivery_thread,
        NULL,
        delivery,
        NULL
    );


    pthread_create(
        &monitor_thread,
        NULL,
        monitor,
        NULL
    );



    pthread_join(kitchen_thread,NULL);

    pthread_join(delivery_thread,NULL);

    pthread_join(monitor_thread,NULL);



    pthread_mutex_destroy(&mutex);

    pthread_cond_destroy(&not_full);

    pthread_cond_destroy(&not_empty);



    return 0;
}
