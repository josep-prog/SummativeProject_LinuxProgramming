# **Project 4: Designing a Multithreaded Order Processing System in C**

## **Explanation, Design, Implementation, and Execution Guide**

# **Introduction**

Modern online food delivery platforms receive many customer orders at the same time. These orders need to be prepared, stored temporarily, and delivered efficiently. If different tasks happen at the same time, the system must carefully manage shared information to avoid errors.

This project implements a simplified food delivery order processing system using **multithreading in C**.

The program simulates three different activities running at the same time:

1. **Kitchen Thread (Producer)**  
    Creates and prepares new orders.  
2. **Delivery Thread (Consumer)**  
    Removes prepared orders and processes deliveries.  
3. **Monitoring Thread**  
    Periodically displays the current system status.

The main challenge of this project is that all threads share the same order queue. Since multiple threads can access the queue simultaneously, synchronization mechanisms are required to prevent incorrect data.

The program uses:

* POSIX Threads (pthread)  
* Mutex locks (pthread\_mutex\_t)  
* Condition variables (pthread\_cond\_t)  
* Shared memory  
* Producer-consumer design pattern

# **1\. Understanding Threads**

A thread is a small execution unit inside a program.

Normally, a program executes tasks one after another:

Start  
 |  
Task 1  
 |  
Task 2  
 |  
End

With multiple threads, different tasks can execute at the same time:

                Program

        \-----------------------  
        |          |          |  
        v          v          v

    Kitchen    Delivery    Monitor  
    Thread      Thread      Thread

In this project:

* The kitchen prepares orders.  
* Delivery processes orders.  
* Monitoring checks the system.

All three activities run independently.

# **2\. Producer-Consumer Model**

This project follows the producer-consumer pattern.

The idea is simple:

* A producer creates data.  
* A consumer uses that data.

In this system:

Kitchen Thread  
     |  
     |  
     v

 Order Queue

     |  
     |  
     v

Delivery Thread

The kitchen produces orders and places them in the queue.

The delivery thread consumes orders from the queue.

# **3\. Shared Queue**

The system uses a shared queue to store prepared orders before delivery.

The queue has a fixed size:

Maximum capacity \= 5 orders

Example:

Queue:

\[Order 1\]\[Order 2\]\[Order 3\]\[ \]\[ \]

Current size \= 3

The queue stores orders temporarily.

The program keeps track of:

* Front position → where orders are removed.  
* Rear position → where new orders are added.  
* Count → number of orders currently waiting.

# **4\. Why Synchronization is Needed**

Because multiple threads run at the same time, they may access the same data simultaneously.

Example:

The kitchen wants to add an order:

Queue size \= 5

At the same time, delivery wants to remove an order.

Without synchronization, both threads may modify the queue at the same time and create incorrect results.

This problem is called a:

## **Race Condition**

A race condition occurs when the result depends on which thread executes first.

To prevent this, the program uses:

* Mutex locks  
* Condition variables

# **5\. Mutex Locks**

A mutex works like a key that allows only one thread to access shared data at a time.

Example:

Thread A gets the lock

      |  
      v

Modify queue

      |  
      v

Release lock

Thread B can now access queue

In C:

Lock:

pthread\_mutex\_lock(\&mutex);

Unlock:

pthread\_mutex\_unlock(\&mutex);

The queue operations are protected using a mutex so that only one thread can modify it at a time.

# **6\. Condition Variables**

Condition variables allow threads to wait until a specific condition becomes true.

The program uses two conditions:

## **not\_full**

Used when the queue is full.

Example:

Queue:

\[1\]\[2\]\[3\]\[4\]\[5\]

No space available

The kitchen cannot add another order.

Therefore:

Kitchen waits

When delivery removes an order:

\[1\]\[2\]\[3\]\[4\]\[ \]

The delivery thread signals the kitchen:

Space available  
Continue working

## **not\_empty**

Used when the queue is empty.

Example:

Queue:

\[ \]\[ \]\[ \]\[ \]\[ \]

No orders available

The delivery thread waits.

When the kitchen adds an order:

\[Order 1\]\[ \]\[ \]\[ \]\[ \]

The kitchen signals delivery:

New order available  
Continue working

# **7\. Kitchen Thread (Producer)**

The kitchen thread creates new orders.

Its responsibilities are:

1. Generate an order ID.  
2. Prepare the order.  
3. Add the order to the queue.

The preparation time is:

2 seconds

Example:

Creating Order 1

Preparing...

Order 1 added to queue

If the queue is full:

Kitchen waits

until delivery creates space.

# **8\. Delivery Thread (Consumer)**

The delivery thread removes orders from the queue.

Its responsibilities are:

1. Check if an order exists.  
2. Remove an order.  
3. Deliver the order.

The delivery time is:

4 seconds

Example:

Taking Order 1

Delivering...

Order 1 completed

If the queue is empty:

Delivery waits

until the kitchen creates a new order.

# **9\. Monitoring Thread**

The monitoring thread observes the system.

It runs every:

5 seconds

It displays:

Orders prepared: X  
Orders delivered: Y  
Queue size: Z

Example:

\--- Monitor \---

Orders prepared: 10  
Orders delivered: 6  
Queue size: 4

\---------------

The monitoring thread also uses the mutex because it reads shared variables.

# **10\. Program Execution Flow**

The complete system works like this:

                Main Program

                       |  
        \--------------------------------  
        |              |               |  
        v              v               v

    Kitchen        Delivery        Monitor

        |              |  
        |              |  
        \------ Queue \------

The process:

1. Main creates three threads.  
2. Kitchen creates orders.  
3. Orders are stored in the queue.  
4. Delivery removes orders.  
5. Monitor displays statistics.  
6. Threads finish after processing all orders.

# **11\. Important pthread Functions Used**

## **Creating Threads**

The function:

pthread\_create()

starts a new thread.

Example:

pthread\_create(  
    \&thread,  
    NULL,  
    function,  
    NULL  
);

## **Waiting for Threads**

The function:

pthread\_join()

waits for a thread to finish.

Example:

pthread\_join(thread,NULL);

## **Mutex Functions**

Lock:

pthread\_mutex\_lock()

Unlock:

pthread\_mutex\_unlock()

## **Condition Variable Functions**

Waiting:

pthread\_cond\_wait()

Sending a signal:

pthread\_cond\_signal()

# **12\. Compilation and Running**

## **Step 1: Save the file**

Save the program as:

order\_system.c

## **Step 2: Compile**

Because the program uses POSIX threads, the \-pthread option is required.

Run:

gcc order\_system.c \-o order\_system \-pthread

This creates the executable:

order\_system

## **Step 3: Run the Program**

Execute:

./order\_system

# **13\. Example Output**

A possible output:

Kitchen prepared order 1

Kitchen prepared order 2

Delivered order 1

\--- Monitor \---

Orders prepared: 2  
Orders delivered: 1  
Queue size: 1

\---------------

Kitchen prepared order 3

Delivered order 2

The output shows that:

* Orders are created.  
* Orders are delivered.  
* Queue size changes.  
* Monitoring works correctly.

# **14\. Memory Protection Explanation**

The queue and counters are shared between threads.

Examples:

queue\[\]  
prepared  
delivered  
count

Because these variables are shared, they are protected by a mutex.

Before accessing shared data:

pthread\_mutex\_lock()

After finishing:

pthread\_mutex\_unlock()

This ensures that two threads cannot modify the same data at the same time.

# **15\. Handling Full and Empty Queue**

## **Full Queue**

Condition:

count \== 5

Action:

Kitchen waits

When space becomes available:

Delivery signals kitchen

## **Empty Queue**

Condition:

count \== 0

Action:

Delivery waits

When a new order arrives:

Kitchen signals delivery

# **Conclusion**

This project demonstrates how real-world systems can be built using multiple threads working together. The food delivery example represents many practical systems where one component produces information while another consumes it.

The implementation demonstrates important operating system concepts:

* Parallel execution using threads.  
* Safe access to shared memory.  
* Synchronization using mutex locks.  
* Communication using condition variables.  
* Producer-consumer architecture.

By using these techniques, the program avoids race conditions and ensures that orders are processed correctly even when multiple tasks are running at the same time.

