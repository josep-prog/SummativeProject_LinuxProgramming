# **Assignment : Summative Project Linux Programing**

Date : 30th July 2026

| Name | Link |
| :---- | :---- |
| **Github repository** | **[Summative Project](https://github.com/josep-prog/SummativeProject_LinuxProgramming.git)**  |
| **DemoVideo\_Summative Project Linux Programing** | **[Complete\_DemoVideo](https://youtu.be/7Sh0B5KqP0c)**  |
| **Project 1**: Investigating an ELF Executable | **[program.c](https://github.com/josep-prog/SummativeProject_LinuxProgramming/blob/main/Project1/program.c)**  |
|  | **[README.md](https://github.com/josep-prog/SummativeProject_LinuxProgramming/blob/main/Project1/README.md)**  |
|  | **[PROJECT1\_demoVideo](https://youtu.be/UPqY9ITWfHI)**  |
| **Project 2:** Assembly-Based Text File Analysis | **[sensor\_analysis.asm](https://github.com/josep-prog/SummativeProject_LinuxProgramming/blob/main/Project2/sensor_analysis.asm)**  |
|  | **[README.md](https://github.com/josep-prog/SummativeProject_LinuxProgramming/blob/main/Project2/README.md)**  |
|  | **[Project2\_DemoVideo](https://youtu.be/YtkWuiqToL0)**  |
| **Project 3** : Developing a Python C Extension for High-Performance Data Processing | **[sensor\_analysis.c](https://github.com/josep-prog/SummativeProject_LinuxProgramming/blob/main/Project3/sensor_analysis.c)**  |
|  | **[setup.py](https://github.com/josep-prog/SummativeProject_LinuxProgramming/blob/main/Project3/setup.py)**  |
|  | **[test.py](https://github.com/josep-prog/SummativeProject_LinuxProgramming/blob/main/Project3/test.py)**  |
|  | **[README.md](https://github.com/josep-prog/SummativeProject_LinuxProgramming/blob/main/Project3/README.md)**  |
|  | **[Project3\_DemoVideo](https://youtu.be/F7JdMwu5rWk)**  |
| **Project 4:** Designing a Multithreaded Order Processing System in C | **[order\_system.c](https://github.com/josep-prog/SummativeProject_LinuxProgramming/blob/main/Project4/order_system.c)**  |
|  | **[README.md](https://github.com/josep-prog/SummativeProject_LinuxProgramming/blob/main/Project4/README.md)**  |
|  | **[Project4\_DemoVideo](https://youtu.be/SuGYQXw-ZZc)**  |
| **Project 5:** Building a Concurrent TCP Client–Server Monitoring System | **[server.c](https://github.com/josep-prog/SummativeProject_LinuxProgramming/blob/main/Project5/server.c)**  |
|  | **[client.c](https://github.com/josep-prog/SummativeProject_LinuxProgramming/blob/main/Project5/client.c)**  |
|  | **[README.md](https://github.com/josep-prog/SummativeProject_LinuxProgramming/blob/main/Project5/README.md)**  |
|  | **[Project5\_DemoVideo](https://youtu.be/ONAP47ju_uE)**  |

# 

# 

# **Project 1: Program Implementation**

## **1\. Program Development**

I designed the program to keep its execution flow simple and easy to follow while investigating it as an ELF executable. Instead of placing all the logic inside main(), I separated the program into three small functions, each responsible for a single task. This made the source code easier to understand and later made it much easier to recognize each function during disassembly and debugging.

To control the amount of data processed by the program, I declared an initialized global variable named globalNumber. This variable is accessed inside main() to determine the size of the array that will be created. Since it is initialized before execution begins, it is stored in the executable's .data section, making it easy to locate later during memory inspection.

int globalNumber \= 5;

Rather than creating an array with a fixed size on the stack, I allocated memory dynamically using malloc(). This allows the program to request only the amount of memory it needs during execution and also provides a practical example of heap memory that can later be examined using debugging tools. Before continuing, the program verifies that the allocation was successful. If no memory is available, it exits safely instead of continuing with an invalid pointer.

int \*numbers \= malloc(size \* sizeof(int));

if (numbers \== NULL)  
{  
    return 1;  
}

Once the memory has been allocated, it needs to contain meaningful values before any calculations can be performed. To achieve this, I implemented the fillArray() function, which uses a simple for loop to visit every position in the allocated array and store sequential values beginning from one. Separating this logic into its own function keeps the implementation organized while making the looping behavior easy to recognize during assembly analysis.

for (int i \= 0; i \< size; i++)  
{  
    arr\[i\] \= i \+ 1;  
}

After initializing the array, the next step is to calculate the total of all stored values. Instead of combining this calculation with other operations, I implemented the sumArray() function. This function iterates through every element, continuously adds each value to a local variable named sum, and returns the final result to main(). Keeping the calculation separate improves readability and clearly defines the responsibility of each function.

for (int i \= 0; i \< size; i++)  
{  
    sum \+= arr\[i\];  
}

The calculated total is then passed to the printResult() function, where a simple decision determines what is displayed on the terminal. If the calculated sum is greater than ten, the exact value is printed; otherwise, a different message is displayed. This small condition provides a clear example of decision-making in C and later appears as a conditional branch in the generated assembly code.

if (sum \> 10\)  
{  
    printf("Sum is %d\\n", sum);  
}  
else  
{  
    printf("Small sum\\n");  
}

Throughout the implementation, several C Standard Library functions are used to perform common operations. The malloc() function allocates memory on the heap, printf() displays output on the terminal, and free() releases the allocated memory before the program exits. Using these library functions demonstrates how user-written code interacts with external shared libraries during execution.

Finally, the execution flow begins inside main(), where the global variable is read, memory is allocated, the array is initialized, the values are summed, the result is displayed, and the allocated memory is released before the program terminates successfully. This sequence creates a straightforward execution path that can easily be followed during static analysis, dynamic analysis, and debugging.

# **2\. Static Analysis**

After compiling and stripping the executable, I began examining the binary using readelf to understand its internal structure. The analysis confirmed that the executable is a 64-bit ELF binary targeting the x86-64 architecture. It also showed that the program is dynamically linked, meaning that functions such as malloc(), printf(), and free() are loaded from the shared C library when the program starts instead of being embedded directly inside the executable.

The ELF section table provided a clear view of how different parts of the program are organized. The .text section contains the executable machine instructions generated from the C source code, while the initialized global variable globalNumber is stored in the .data section. If the program contained uninitialized global variables, they would appear in the .bss section. The .plt (Procedure Linkage Table) and .got (Global Offset Table) work together to connect the executable with external library functions during runtime by resolving their addresses whenever they are called.

Because the executable had already been stripped, the original function names were removed from the symbol table. However, imported library functions remained visible because the dynamic linker still requires them during execution. This confirms why functions such as malloc(), printf(), and free() can still be identified even after the executable has been stripped.

To understand how the original C program was translated into machine instructions, I used objdump to disassemble the executable. Instead of relying on function names, I reconstructed each function by following its instruction sequence, parameters, loops, and return values. This made it possible to identify the behavior of the initialization function, the summation function, the output function, and the main() function directly from the generated assembly code.

The disassembly also revealed how high-level programming constructs are represented at the machine level. The for loop appears as a sequence of instructions that initialize the loop counter, compare it with the array size, execute the loop body, increment the counter, and repeat until the condition is no longer true. Likewise, the if-else statement inside printResult() appears as a comparison instruction followed by a conditional jump that directs execution to the appropriate output statement.

# **3\. Dynamic Analysis**

To observe the program while it was running, I executed the stripped binary using strace. This allowed me to monitor every system call made between the application and the Linux kernel throughout its execution.

The execution begins with several startup system calls responsible for loading the executable and preparing the runtime environment. During memory allocation, the C library internally requests memory from the operating system using memory management system calls such as brk() and mmap(). Although the source code only calls malloc(), these lower-level system calls demonstrate how the operating system actually provides heap memory to the program.

When the program displays its result, the printf() function eventually performs a write() system call to send the formatted text to the terminal. Finally, after the allocated memory has been released using free(), the program terminates by invoking exit\_group(), completing the execution process.

Observing these system calls provides a clear connection between the high-level C functions used in the implementation and the operating system services that actually perform the requested operations.

# **4\. Debugging and Memory Inspection**

To examine the program during execution, I used GDB to debug the stripped executable. I first placed a breakpoint at the program entry point (start) to observe where execution begins before reaching my own code. Continuing execution moved the program into main(), confirming the transition from the runtime startup code into the application itself.

A second breakpoint was placed inside main(), allowing me to inspect the variables created during execution. At this point, the value stored in the global variable globalNumber was available and was used to initialize the local variable size, confirming that the program correctly retrieved the initialized global data before allocating memory.

To observe function execution more closely, I placed another breakpoint inside fillArray(). Entering this function confirmed that the dynamically allocated array pointer and the array size were successfully passed as function parameters. This also verified that the function identified during static analysis was indeed executed at runtime.

Using the backtrace command, I displayed the call stack to observe the current execution path. Initially, the stack showed that execution had entered main(). After moving into fillArray(), the call stack reflected the function call made from main(), illustrating how GDB records the sequence of function calls during program execution.

Finally, I inspected the three primary memory regions used by the application. The initialized global variable was located in the global data segment, the dynamically allocated array was stored on the heap after malloc() completed successfully, and local variables such as size and the loop counter were stored on the stack while each function was executing. Examining these regions provided a practical demonstration of how global memory, heap memory, and stack memory are organized and used throughout the lifetime of the program.

# **Project 2: Assembly-Based Text File Analysis**

## **Program Implementation**

The program was developed to analyze a text file named sensor\_readings.txt by reading its contents, examining every character, and determining both the total number of records and the number of valid records. To make the implementation organized and easier to follow, the program is divided into three main parts: preparing the required data, processing the file, and displaying the final results.

The implementation begins in the .data section, where the resources needed during execution are prepared. This includes the input file name, the messages displayed to the user, and the error messages used whenever file operations fail. Defining these values before execution allows the operating system to receive the exact file name when the program requests access to the input file.

section .data

filename db "sensor\_readings.txt", 0

msg\_total db "Total records: "

msg\_valid db "Valid records: "

The .bss section reserves memory that will only be used while the program is running. A buffer is allocated to temporarily store the contents of the input file after it has been read from disk. Additional variables are also reserved to keep track of the total number of lines, the number of valid records, and a flag used to determine whether the current line contains any data before a newline character is reached.

buffer      resb 1024

total\_lines resq 1

valid\_lines resq 1

has\_data    resb 1

Program execution begins at \_start, where the first task is to open the input file. Since the program only needs to analyze the contents without modifying them, the Linux sys\_open system call is performed in read-only mode. If the operating system cannot open the file, execution immediately branches to the error handling routine, where an appropriate error message is displayed before the program terminates safely.

mov rax, 2

mov rdi, filename

mov rsi, 0

syscall

cmp rax, 0

jl open\_error

After successfully opening the file, the next step is to load its contents into memory. This is achieved using the sys\_read system call, which copies the data from the file into the previously allocated buffer. Processing the data from memory instead of directly from the file makes it possible to examine every character individually during the analysis stage. The implementation also verifies that the read operation completes successfully before continuing.

mov rax, 0

mov rdi, r12

mov rsi, buffer

mov rdx, 1024

syscall

cmp rax, 0

jl read\_error

Once the file has been loaded into memory, the program initializes all counters before beginning the analysis. Both the total record counter and the valid record counter start from zero, while the has\_data flag is cleared to indicate that no characters have yet been found on the current line.

The core of the implementation is the character traversal loop. Instead of processing complete lines at once, the program moves through the buffer one character at a time until every byte returned by sys\_read has been examined. A pointer stored in RSI always references the current character, while RCX keeps track of the number of characters remaining to be processed. This approach allows the program to accurately detect line boundaries regardless of the file's contents.

mov rsi, buffer

mov rcx, r13

loop\_chars:

cmp rcx, 0

je finish

mov al, \[rsi\]

As each character is inspected, conditional branches determine whether it represents normal data or the end of a record. Whenever an ordinary character is encountered, the has\_data flag is set to indicate that the current line contains valid information. If the program encounters a newline character (LF), it recognizes that the current record has ended. At this point, the total number of records is increased, and the has\_data flag is checked to determine whether the line should also be counted as a valid record. Empty lines are therefore counted as records but are excluded from the valid record total.

cmp al, 10

je new\_line

mov byte \[has\_data\], 1

To improve compatibility with different operating systems, the implementation also recognizes Windows-style line endings. Whenever a carriage return (CR) is encountered, it is ignored so that the following newline character can still be treated as the actual end of the record. This enables the same program to correctly process files that use either Unix (LF) or Windows (CRLF) line endings.

cmp al, 13

je next\_character

Another situation handled by the implementation occurs when the final line of the file does not end with a newline character. Before printing the results, the program performs one final check on the has\_data flag. If data was found on the last line but no terminating newline exists, that line is still counted as both a total record and a valid record. This prevents the final measurement from being ignored simply because the file does not end with a line break.

After completing the analysis, the calculated totals are displayed on the terminal. Unlike higher-level languages such as C, assembly language does not provide built-in output functions. Instead, the implementation relies on the Linux sys\_write system call to display messages. Since system calls can only output text, a separate routine named print\_number converts the calculated integer values into their ASCII representation before writing them to the console.

mov rax, \[total\_lines\]

call print\_number

mov rax, \[valid\_lines\]

call print\_number

The print\_number routine repeatedly divides the number by ten, converts each digit into its corresponding ASCII character, stores the digits in reverse order inside a temporary buffer, and finally writes the completed string to the terminal using sys\_write. This routine allows any calculated value to be displayed correctly without relying on external libraries.

To improve the reliability of the application, dedicated error-handling routines are included for both file opening and file reading operations. If the input file cannot be opened, or if an error occurs while reading its contents, the program immediately prints a descriptive error message to the standard error stream before terminating with a non-zero exit status. Handling these situations separately ensures that the user receives meaningful feedback instead of allowing the program to continue with invalid data.

Finally, after all processing has been completed and the results have been displayed successfully, the program terminates using the Linux sys\_exit system call. This provides a clean and predictable end to execution while properly returning control to the operating system.

# **Project 3: Developing a Python C Extension for High-Performance Data Processing**

## **Program Implementation**

The objective of this project was to improve the performance of numerical data processing by implementing the computationally intensive operations directly in C while still allowing them to be used from Python. Instead of performing all calculations in Python, I developed a native extension module named **sensor\_analysis** using the Python C API. This approach combines the execution speed of C with the simplicity of Python, making it suitable for processing large collections of sensor readings efficiently.

The implementation is organized into three main files. The sensor\_analysis.c file contains the native C implementation of every statistical function together with the module definition. The setup.py file is responsible for compiling the C source into a Python extension module, while test.py imports the module, executes every implemented function, and verifies both normal and invalid input cases.

## **Accessing Python Objects from C**

Since Python objects cannot be used directly as C values, the implementation begins by converting Python numeric objects into C double values before any calculations are performed. To avoid repeating the same conversion logic throughout the program, I implemented a helper function named as\_double(). This function uses the Python C API function PyFloat\_AsDouble() to extract the numeric value and immediately checks whether the conversion generated an error. If an invalid object is supplied, the function returns without continuing the calculation, ensuring that only valid numeric values are processed.

static int as\_double(PyObject \*item, double \*out)

{

    double value \= PyFloat\_AsDouble(item);

    if (value \== \-1.0 && PyErr\_Occurred())

        return 0;

    \*out \= value;

    return 1;

}

Using this helper function ensures that all statistical operations perform their calculations using the C double data type while safely handling invalid values returned from Python.

## **Input Validation and Dataset Verification**

Before any statistical calculation begins, the input dataset is validated by the get\_data() function. Rather than performing the same validation inside every statistical function, this common routine centralizes all input checking.

The implementation first verifies that the supplied argument is either a Python list or tuple using the Python C API. It then retrieves the dataset size and confirms that at least one value is available for processing. Whenever validation fails, the function raises the appropriate Python exception, allowing Python programs to receive clear error messages instead of unexpected behaviour.

if (\!PyList\_Check(data) && \!PyTuple\_Check(data))

{

    PyErr\_SetString(PyExc\_TypeError,

                    "Input must be a list or tuple");

    return 0;

}

if (\*size \== 0\)

{

    PyErr\_SetString(PyExc\_ValueError,

                    "Dataset cannot be empty");

    return 0;

}

Centralizing the validation logic reduces code duplication while ensuring that every statistical function performs exactly the same safety checks before beginning its calculations.

## **Average Calculation**

The average() function computes the arithmetic mean of the supplied sensor readings. After validating the input, the function traverses the dataset once, converts each Python object into a C double, accumulates the total, and divides the final sum by the total number of samples.

\[  
\\text{Average}=\\frac{\\sum x}{n}  
\]

The calculation is performed entirely in C using the double data type before the result is converted back into a Python floating-point object using PyFloat\_FromDouble(). Since each element is processed only once, the implementation has a time complexity of **O(n)**.

sum \+= value;

return PyFloat\_FromDouble(sum / size);

## **Range Calculation**

The range\_value() function determines the difference between the largest and smallest sensor reading.

Instead of sorting the dataset, which would require additional processing, the implementation initializes both the minimum and maximum values using the first element of the dataset. The remaining values are then scanned once, updating the minimum and maximum whenever a smaller or larger value is found.

\[  
\\text{Range}=Maximum-Minimum  
\]

After completing the traversal, the function returns the calculated range as a Python floating-point value. Processing the dataset only once keeps the overall complexity at **O(n)** while avoiding unnecessary operations.

## **Variance Calculation**

The variance() function computes the **sample variance** of the sensor readings.

Before performing the calculation, the implementation verifies that the dataset contains at least two samples because sample variance cannot be calculated from a single value. If this condition is not satisfied, a ValueError exception is raised.

The calculation follows the standard sample variance formula:

\[  
Variance=\\frac{\\sum(x-\\bar{x})^2}{n-1}  
\]

To produce the result, the implementation first performs one traversal to calculate the arithmetic mean. A second traversal then computes the squared difference between each reading and the mean before dividing the accumulated total by *(n − 1\)*.

Although the dataset is scanned twice, each traversal is linear, resulting in an overall time complexity of **O(n)** while maintaining numerical accuracy for the calculated variance.

---

## **Counting Values Above a Threshold**

The count\_above() function determines how many sensor readings exceed a user-specified threshold.

After validating the input dataset, each value is converted into a C double and compared against the supplied limit. Whenever a value is greater than the threshold, the counter is increased. Once every reading has been processed, the final count is returned to Python as an integer object.

if(value \> limit)

    count++;

return PyLong\_FromLong(count);

## **Returning Multiple Statistics**

To avoid making several function calls for commonly requested information, the implementation provides a statistics() function that calculates multiple values together and returns them as a Python dictionary.

During the dataset traversal, the implementation determines the total number of samples, computes the running sum, and continuously updates both the minimum and maximum values. Once processing is complete, the calculated values are inserted into a Python dictionary using PyDict\_SetItemString().

The returned dictionary contains the following information:

{

    "samples": ...,

    "average": ...,

    "minimum": ...,

    "maximum": ...

}

Returning all related statistics together makes the module easier to use from Python while reducing the need for multiple function calls.

## **Memory Management**

The implementation performs all numerical calculations directly on the Python sequence without allocating additional arrays or temporary buffers. Each element is accessed individually using PySequence\_GetItem(), converted into a C double, processed immediately, and its reference is released using Py\_DECREF() before moving to the next element.

PyObject \*item \= PySequence\_GetItem(data, i);

...

Py\_DECREF(item);

Because every value is processed as it is read, no duplicate copy of the dataset is created in memory. This minimizes memory usage and satisfies the requirement to avoid unnecessary dynamic memory allocation.

## **Returning Results to Python**

After completing each calculation, the implementation converts the C result back into the appropriate Python object before returning it to the caller. Floating-point results such as the average, range, and variance are returned using PyFloat\_FromDouble(), integer values are returned using PyLong\_FromLong(), and the combined statistics are returned as a Python dictionary.

Using the correct Python object types ensures that the extension module behaves exactly like a native Python module while still performing all calculations internally in C.

## **Registering the Extension Module**

Once all statistical functions were implemented, they were registered inside the module's method table. Each entry associates the Python function name with its corresponding C implementation, allowing Python to locate and execute the correct function whenever the module is imported.

static PyMethodDef methods\[\] \=

{

    {"average", average, METH\_VARARGS, "Calculate average"},

    {"range\_value", range\_value, METH\_VARARGS, "Calculate range"},

    {"variance", variance, METH\_VARARGS, "Calculate variance"},

    {"count\_above", count\_above, METH\_VARARGS, "Count values above limit"},

    {"statistics", statistics, METH\_VARARGS, "Return statistics dictionary"},

    {NULL, NULL, 0, NULL}

};

The module definition and initialization function complete the implementation by creating the Python extension module named sensor\_analysis, making every registered function available for use within Python applications.

## **Building the Extension Module**

To compile the C source into a Python extension module, a setup.py file was created using setuptools. The build configuration specifies the extension name and the C source file to compile.

module \= Extension(

    "sensor\_analysis",

    sources=\["sensor\_analysis.c"\]

)

This configuration allows the module to be built successfully and imported like any standard Python package.

## **Testing the Implementation**

A separate Python test program was developed to verify that every implemented function operates correctly. The script imports the compiled extension module, creates a sample dataset of floating-point sensor readings, and calls each statistical function individually. The returned results are displayed on the console to confirm that the calculations performed in C are correctly returned to Python.

In addition to normal execution, the test program also verifies boundary conditions by passing an empty dataset to the average() function. As expected, the extension raises a ValueError exception, confirming that invalid input is detected and handled safely instead of allowing undefined behaviour.

This testing process demonstrates that the extension module correctly integrates with Python, performs all numerical processing directly in C, validates input, handles errors appropriately, and returns valid Python objects for every implemented operation.

# **Project 4: Designing a Multithreaded Order Processing System in C**

## **Program Implementation**

The objective of this project was to simulate a concurrent order processing system using POSIX threads while ensuring that multiple threads could safely communicate through shared memory. The implementation models an online food delivery platform where one thread prepares customer orders, another thread processes them for delivery, and a third thread continuously monitors the overall system. To prevent inconsistent data and race conditions, all shared resources are synchronized using mutex locks and condition variables.

The implementation begins by defining a fixed-size shared queue capable of storing five orders at a time. Each order contains a unique identifier, while additional shared variables keep track of the queue position, the current number of orders stored, and the total numbers of prepared and delivered orders. These shared variables are accessed by multiple threads throughout execution, making synchronization essential.

\#define SIZE 5

\#define ORDERS 20

Order queue\[SIZE\];

int front \= 0;

int rear \= 0;

int count \= 0;

To coordinate access to the shared queue, the implementation uses one mutex and two condition variables. The mutex protects all shared data from simultaneous modification, while the condition variables allow threads to wait efficiently whenever the queue becomes either full or empty.

pthread\_mutex\_t mutex;

pthread\_cond\_t not\_full;

pthread\_cond\_t not\_empty;

Using these synchronization objects ensures that only one thread accesses the shared queue at a time while allowing other threads to continue immediately when the required condition becomes true.

## **Shared Queue Implementation**

Communication between the kitchen and delivery threads takes place through a circular shared queue. Two helper functions were implemented to simplify queue operations.

The add\_order() function inserts a newly prepared order into the next available position and updates both the rear index and the current queue size. Similarly, the remove\_order() function removes the oldest order from the front of the queue before updating the front index and decreasing the queue size.

void add\_order(Order order)

{

    queue\[rear\] \= order;

    rear \= (rear \+ 1\) % SIZE;

    count++;

}

Order remove\_order()

{

    Order order \= queue\[front\];

    front \= (front \+ 1\) % SIZE;

    count--;

    return order;

}

Using a circular queue allows the implementation to reuse queue positions efficiently while ensuring that the number of stored orders never exceeds the predefined capacity.

**Kitchen Thread (Producer)**

The kitchen thread implements the producer component of the system. Its responsibility is to generate customer orders continuously, assign each order a unique sequential identifier, and place the completed order into the shared queue.

To simulate food preparation, the thread pauses for two seconds before creating each order.

sleep(2);

Order order;

order.id \= i;

Before inserting a new order into the queue, the kitchen thread locks the shared mutex. If the queue has already reached its maximum capacity, the producer cannot continue because no additional space is available. Instead of repeatedly checking the queue, the thread waits on the not\_full condition variable until the delivery thread removes an order and signals that space is available.

pthread\_mutex\_lock(\&mutex);

while(count \== SIZE)

{

    pthread\_cond\_wait(\&not\_full, \&mutex);

}

Once space becomes available, the order is inserted into the queue, the number of prepared orders is updated, and the producer signals the not\_empty condition variable to notify the delivery thread that a new order is ready for processing.

add\_order(order);

prepared++;

pthread\_cond\_signal(\&not\_empty);

pthread\_mutex\_unlock(\&mutex);

This synchronization guarantees that the producer never inserts more than five orders into the shared queue and automatically resumes execution whenever additional space becomes available.

## **Delivery Thread (Consumer)**

The delivery thread implements the consumer component of the system. Its responsibility is to remove prepared orders from the shared queue and simulate their delivery.

Before attempting to remove an order, the delivery thread first locks the mutex protecting the shared queue. If the queue is empty, no order is available for delivery. Rather than continuously checking for new orders, the thread waits on the not\_empty condition variable until the kitchen thread produces another order.

pthread\_mutex\_lock(\&mutex);

while(count \== 0\)

{

    pthread\_cond\_wait(\&not\_empty, \&mutex);

}

After removing an order from the queue, the number of delivered orders is updated. The delivery thread then signals the not\_full condition variable to inform the producer that another queue position has become available before releasing the mutex.

Order order \= remove\_order();

delivered++;

pthread\_cond\_signal(\&not\_full);

pthread\_mutex\_unlock(\&mutex);

The delivery itself is simulated by pausing for four seconds before printing the completed order.

sleep(4);

printf("Delivered order %d\\n", order.id);

This implementation guarantees that no thread attempts to remove an order from an empty queue while maintaining correct synchronization with the producer.

## **Monitoring Thread**

To observe the system while it is running, a third thread was implemented as a monitoring thread. Unlike the producer and consumer, this thread does not modify the queue. Instead, it periodically reports the current system status every five seconds.

Before accessing any shared information, the monitoring thread locks the same mutex used by the producer and consumer. This guarantees that the displayed statistics remain consistent and cannot be modified while they are being read.

pthread\_mutex\_lock(\&mutex);

printf("Orders prepared: %d\\n", prepared);

printf("Orders delivered: %d\\n", delivered);

printf("Queue size: %d\\n", count);

pthread\_mutex\_unlock(\&mutex);

The monitoring thread continues running until all customer orders have been delivered, after which it exits normally without interfering with the producer-consumer operations.

## **Thread Creation and Management**

The program begins execution in the main() function, where all synchronization objects are first initialized before any threads are created.

Three independent threads are then created using pthread\_create(). Each thread begins executing its own function concurrently: the kitchen thread produces orders, the delivery thread processes completed orders, and the monitoring thread periodically reports the system status.

pthread\_create(\&kitchen\_thread, NULL, kitchen, NULL);

pthread\_create(\&delivery\_thread, NULL, delivery, NULL);

pthread\_create(\&monitor\_thread, NULL, monitor, NULL);

Since the operating system schedules each thread independently, all three activities execute concurrently instead of one after another.

After creating the threads, the main() function waits for every thread to complete by calling pthread\_join(). This guarantees that the program does not terminate while any thread is still executing.

pthread\_join(kitchen\_thread, NULL);

pthread\_join(delivery\_thread, NULL);

pthread\_join(monitor\_thread, NULL);

Once execution has finished, all synchronization objects are destroyed before the program exits, releasing the resources allocated for multithreading.

## **Shared Memory Protection**

Because the producer, consumer, and monitoring threads all access the same shared queue and shared counters, protecting shared memory is essential.

The implementation uses a single mutex to ensure that only one thread can modify or inspect the shared queue at any given time. Every operation involving the queue or shared counters begins by acquiring the mutex and ends by releasing it after the operation has completed.

This approach prevents race conditions, eliminates inconsistent updates, and guarantees that shared data always remains in a valid state throughout program execution.

## **Thread Coordination Using Condition Variables**

The implementation uses two condition variables to coordinate communication between the producer and consumer threads.

The not\_full condition variable causes the kitchen thread to wait whenever the shared queue reaches its maximum capacity. Once the delivery thread removes an order, it signals the producer so that order preparation can continue.

Similarly, the not\_empty condition variable causes the delivery thread to wait whenever no prepared orders are available. After the kitchen thread inserts a new order into the queue, it signals the consumer to continue processing.

Using condition variables allows the threads to sleep while waiting instead of repeatedly checking the queue, resulting in efficient synchronization and preventing unnecessary CPU usage.

## **Program Execution**

For testing purposes, the implementation processes a fixed set of twenty customer orders. During execution, the kitchen thread continuously prepares orders every two seconds, the delivery thread processes completed orders every four seconds, and the monitoring thread reports the current system status every five seconds.

The execution output demonstrates that orders are created successfully, delivered in sequence, the queue size changes dynamically as orders are added and removed, and periodic monitoring reports accurately display the numbers of prepared orders, delivered orders, and the current queue size. Throughout execution, the queue never exceeds its maximum capacity, no order is removed from an empty queue, and all shared resources remain correctly synchronized using mutex locks and condition variables.

# **Project 5 Report: Building a Concurrent TCP Client Server Monitoring System**

## **Introduction**

This project implements a concurrent TCP client–server monitoring system in C for Linux. The objective was to develop a distributed application that allows multiple clients to connect to a central server simultaneously, authenticate using registered user IDs, view available laboratory equipment, and reserve equipment safely.

The implementation combines TCP socket programming, POSIX threads, and mutex synchronization to ensure that multiple clients can communicate with the server concurrently while preventing race conditions and maintaining consistent shared data.

# **System Design**

The project consists of two programs:

* **server.c** – manages client connections, authentication, equipment reservations, and shared resources.  
* **client.c** – connects to the server, authenticates the user, displays available equipment, submits reservation requests, and displays server responses.

Each client communicates independently with the server through a dedicated thread created by the server.

# **Implementation**

## **1\. TCP Socket Communication**

The first requirement was to implement communication using TCP sockets.

This was achieved by creating a TCP socket on both the server and the client. The server binds to port **8080**, listens for incoming connections, and accepts client requests continuously. The client creates its own socket and connects to the server using the same port.

This implementation allows reliable communication between clients and the server throughout the reservation process.

## **2\. Supporting Multiple Simultaneous Clients**

Another important requirement was allowing multiple clients to use the server at the same time.

This was implemented using **POSIX threads (pthread)**. Whenever a new client connects, the server creates a new thread using pthread\_create(). Each thread executes the client\_handler() function independently.

Because every client has its own execution thread, multiple users can authenticate, view equipment, and reserve devices simultaneously without blocking one another.

## **3\. User Authentication**

The project required that only registered users should access the equipment reservation system.

To satisfy this requirement, the server stores a predefined list of valid user IDs. When a client connects, the entered user ID is compared against this list by the authenticate() function.

If the user ID exists, the server grants access and sends the available equipment list.

If the ID is not registered, the server immediately returns **LOGIN FAILED** and closes the session.

This ensures that unauthorized users cannot access equipment information.

**4\. Maintaining Connected Users**

The server is also required to maintain and display the list of currently connected users.

This was implemented using the connected\_users array together with the add\_connected\_user() and remove\_connected\_user() functions.

Whenever authentication succeeds, the user is added to the connected users list.

When the client disconnects, the user is removed automatically.

The server displays the connected users whenever the system status changes, allowing active sessions to be monitored easily.

## **5\. Equipment Reservation Management**

The server maintains an array containing all laboratory equipment together with another array indicating whether each item is available or already reserved.

After successful authentication, the server sends the equipment list to the client.

The client selects one item and sends the reservation request back to the server.

The server then checks the reservation status.

If the equipment is available, it is marked as reserved and the client receives a reservation confirmation.

If another client has already reserved the same equipment, the request is rejected and the client receives an appropriate message.

This implementation satisfies the reservation management requirement while preventing duplicate reservations.

## **6\. Shared Resource Protection**

Since multiple client threads may attempt to modify reservation data at the same time, the shared resources must be protected.

This requirement was implemented using a **pthread mutex**.

Before accessing shared data such as:

* equipment reservations,  
* connected users,  
* reservation status,

the server locks the mutex using pthread\_mutex\_lock().

Once the shared operation is completed, the mutex is released using pthread\_mutex\_unlock().

This synchronization mechanism prevents race conditions and guarantees consistent reservation records.

## **7\. Reliable Message Exchange**

TCP guarantees reliable byte delivery but does not preserve message boundaries. A single message may arrive in several pieces, or multiple messages may arrive together.

To satisfy the requirement for reliable communication, helper functions were implemented:

* send\_all()  
* recv\_all()  
* send\_msg()  
* recv\_msg()

Every message is transmitted using a **4-byte length prefix** followed by the message data.

The receiver first reads the message length and then continues reading until the complete message has been received.

This guarantees that client and server always receive complete messages correctly regardless of how TCP delivers the data.

## **8\. Independent Client Sessions**

Each client session operates independently from other connected clients.

The client\_handler() function manages:

* authentication,  
* equipment transmission,  
* reservation processing,  
* reservation response,  
* client disconnection.

Because every client is processed in its own thread, one client's activity never interrupts another client's session.

## **9\. Handling Unexpected Client Disconnections**

The server must remain operational even if clients disconnect unexpectedly.

This requirement was satisfied by checking the return values of all socket receive operations.

Whenever a client disconnects unexpectedly:

* the server detects the failure,  
* removes the authenticated user from the connected users list,  
* closes the client socket,  
* continues serving other connected clients.

This prevents server crashes caused by unexpected network interruptions.

## **10\. Error Handling**

Several error handling mechanisms were implemented throughout the project.

The server checks for failures during:

* socket creation,  
* bind(),  
* listen(),  
* accept(),  
* memory allocation,  
* thread creation,  
* message transmission.

The client also validates:

* socket creation,  
* server connection,  
* sending requests,  
* receiving responses.

Whenever an error occurs, an informative message is displayed and resources are released safely before terminating the affected connection.

# **Challenges and Solutions**

One challenge encountered was that TCP does not preserve message boundaries. During testing, some messages arrived merged together while others arrived only partially.

This problem was solved by implementing a length-prefixed communication protocol using the helper functions send\_all(), recv\_all(), send\_msg(), and recv\_msg(). These functions ensure that every message is received completely before processing.

Another challenge was preventing multiple clients from reserving the same equipment simultaneously. This was resolved by protecting all reservation operations with mutex locks so that only one thread can modify the reservation data at any given time.

