# 

| Name | Link |
| :---- | :---- |
| **DemoVideo** | **[Project 5: Building a Concurrent TCP Client–Server Monitoring System](https://youtu.be/ONAP47ju_uE)**  |
|  |  |

# 

# **Project 5: Building a Concurrent TCP Client–Server Monitoring System**

## **Explanation, Design, Implementation, and Execution Guide**

Many modern systems require multiple users to connect to the same service at the same time. Examples include online booking systems, banking systems, university portals, and equipment management platforms.

This project develops a **client-server monitoring system** using:

* TCP socket programming  
* Multiple client connections  
* POSIX threads  
* Mutex synchronization  
* Shared resource management

The system simulates a university laboratory equipment booking platform.

Students and researchers can connect to a central server, authenticate themselves, view available equipment, and reserve a device.

The main challenge is allowing many users to access the server simultaneously while preventing conflicts.

For example:

Two students should not be able to reserve the same microscope at the same time.

The server must coordinate all requests safely.

# **1\. Client-Server Architecture**

The system follows a client-server model.

The architecture looks like this:

                Server

        \-------------------------  
        |                       |  
        |                       |  
     Client 1                Client 2

        |  
        |  
     Client 3

        |  
        |  
     Client 4

The server is the central point that manages:

* users  
* equipment  
* reservations  
* client connections

Clients only send requests and receive responses.

# **2\. What is TCP?**

TCP (Transmission Control Protocol) is a communication protocol that allows two computers to exchange data reliably over a network.

TCP provides:

* reliable delivery  
* ordered messages  
* error checking  
* connection management

Before communication starts:

Client                Server

  |                     |  
  |---- Connect \-------\>|  
  |                     |  
  |\<--- Accept \--------|  
  |                     |  
  |---- Request \------\>|  
  |                     |  
  |\<--- Response \------|

The client first creates a connection with the server.

After the connection is established, both sides can exchange messages.

# **3\. Server Responsibilities**

The server is responsible for managing the whole system.

Its main tasks are:

1. Accept client connections.  
2. Authenticate users.  
3. Provide equipment information.  
4. Process reservation requests.  
5. Protect shared data.

# **4\. Multiple Client Support**

The server must support at least:

5 simultaneous clients

This means several users can connect at the same time.

Example:

Student A  \----\\  
Student B  \-----\\  
Student C  \------\> Server  
Student D  \-----/  
Student E  \----/

A simple way to achieve this is using POSIX threads.

For every new client:

New client connects

        |  
        v

Create new thread

        |  
        v

Handle client separately

Example:

pthread\_create()

Each client gets its own thread.

# **5\. Server Thread Model**

The server works like this:

Main Server Thread

        |  
        |  
Accept connection

        |  
        |  
Create Client Thread

        |  
        |  
Client Thread handles:

\- Login  
\- Equipment list  
\- Reservation  
\- Logout

This allows clients to work independently.

# **6\. User Authentication**

Before accessing equipment information, users must log in.

The server maintains a list of registered users.

Example:

Users:

ID001  
ID002  
ID003

Client sends:

Login: ID001

Server checks:

Is ID001 registered?

YES  
 |  
Allow access

NO  
 |  
Reject connection

Unauthorized users cannot view equipment.

# **7\. Equipment Management**

The server stores available equipment.

Example:

Equipment List:

1\. Microscope  
2\. Laptop  
3\. Camera  
4\. Sensor Kit

Each item has a status:

Available  
Reserved

Example:

Microscope   Available

Laptop       Reserved

Camera       Available

# **8\. Reservation Process**

The process:

### **Step 1**

Client selects equipment:

Reserve Laptop

### **Step 2**

Server checks availability.

Example:

Laptop status?

Available

### **Step 3**

Server reserves it.

Laptop \= Reserved

### **Step 4**

Server sends response:

Reservation successful

# **9\. Preventing Double Reservations**

The most important challenge is preventing two users from reserving the same equipment.

Example problem:

Two users request a microscope:

User A:  
Is microscope available?  
YES

User B:  
Is microscope available?  
YES

Both reserve it.

**Result:**

Microscope reserved twice

This is incorrect.

# **10\. Using Mutex Locks**

A mutex protects shared resources.

The equipment list is shared data.

**Example:**

Before checking reservation: pthread\_mutex\_lock(\&equipment\_mutex);

**Check and update:**

Microscope available?  
Reserve microscope

**Release:**

pthread\_mutex\_unlock(\&equipment\_mutex);

Now only one client can modify equipment at a time.

# **11\. Shared Server Data**

The server stores information such as:

## **Connected users**

Example:

Currently connected:

ID001  
ID004  
ID007

## **Equipment status**

Example:

Microscope : Reserved  
Laptop     : Available  
Camera     : Available

Because multiple threads access this data, it must be protected.

# **12\. Handling Client Disconnection**

Clients may disconnect unexpectedly.

**Example:**

Client closes suddenly

The server should not crash.

The server checks communication results.

**If:**

recv() returns 0

The server knows the client disconnected.

Then:

1. Close socket.  
2. Remove user from connected list.  
3. End client thread.

# **13\. Client Application**

The client performs simpler tasks.

Its responsibilities:

1. Connect to server.  
2. Send user ID.  
3. Receive authentication result.  
4. View equipment.  
5. Request reservation.  
6. Close connection.

# **14\. Client Workflow**

The client process:

Start

 	|

Connect to server

 |

Send user ID

 |

Receive authentication

 |

Receive equipment list

 |

Choose equipment

 |

Receive reservation result

 |

Close session

# **15\. Example Client Session**

**Client:**

Enter User ID: ID001

**Server:**

Authentication successful

Client receives:

Available equipment:

1\. Microscope  
2\. Camera  
3\. Laptop

**Client:**

Select: 1

Server: Microscope successfully reserved

Client exits: Session closed. Goodbye, ID001

# **16\. Important Socket Functions**

## **Server Side**

### **Create socket**

socket()

Creates a communication endpoint.

### **Bind socket**

bind()

Connects the socket to an IP address and port.

### **Listen**

listen()

Waits for incoming connections.

### **Accept**

accept()

Accepts a client connection.

### **Receive data**

recv()

Reads messages from clients.

### **Send data**

send()

Sends responses.

## **Client Side**

### **Connect**

connect()

Connects to the server.

# **17\. Example Program Structure**

A simple project structure:

project5/

│  
├── server.c  
│  
├── client.c  
│  
└── README.txt

# **18\. Server Execution**

Compile:

gcc server.c \-o server \-pthread

Run:

./server

Output:

Server started 

Waiting for clients…

# **19\. Client Execution**

Compile: gcc client.c \-o client

Run: ./client

Output: Connected to server

Enter User ID: ID001

Login successful

Available equipment:  
1\. Microscope  
2\. Laptop  
3\. Camera

Choose equipment:

# **20\. Example Server Output**

Server started on port 8080

Client connected:  
ID001

ID001 reserved Microscope

Current Equipment Status:

Microscope : Reserved  
Laptop     : Available  
Camera     : Available

Client disconnected: ID001

# **21\. Important Concepts Demonstrated**

This project tests understanding of:

* ## **TCP Networking :** Communication between different computers.

* ## **Threads :** Handling multiple clients simultaneously.

* ## **Shared Memory :** Multiple threads accessing common data.

* ## **Mutex Synchronization :** Preventing conflicting operations.

* ## **Resource Management :** Managing equipment availability.

# **22\. Simplest Implementation Plan**

build it in this order:

### **Step 1**

Create basic TCP server.

Test:

Client connects  
Server responds

### **Step 2**

Add user authentication.

Use a simple array:

users\[\] \= {"ID001","ID002"};

### **Step 3**

Add equipment list.

Example:

equipment\[3\];

### **Step 4**

Add reservation logic.

Check:

Available?  
 |  
Yes \-\> Reserve  
 |  
No \-\> Reject

### **Step 5**

Add threads.

Each client gets:

pthread\_create()

### **Step 6**

Add mutex protection.

Protect:

equipment status  
connected users

# 

# **Conclusion**

This project demonstrates how real-world distributed systems are built. The server acts as a central manager while multiple clients communicate with it simultaneously.

The main technical challenges are not only creating network communication but also managing many users safely at the same time.

By combining TCP sockets, threads, and synchronization mechanisms, the system can support multiple users while preventing errors such as duplicate reservations or unauthorized access.

The project introduces important operating system and networking concepts that are used in real applications such as booking platforms, banking systems, cloud services, and online management systems.