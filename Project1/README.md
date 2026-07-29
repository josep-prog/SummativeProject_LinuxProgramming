# 

| Name  | Link |
| :---- | :---- |
| **DemoVideo** | **[Project 1: Investigating an ELF Executable](https://youtu.be/UPqY9ITWfHI)**  |
|  |  |
|  |  |

# 

# **Investigating an ELF Executable: Compilation and Static Analysis**

## 

An ELF (Executable and Linkable Format) file is the standard format used by Linux systems for executable programs, object files, and shared libraries. When we write a C program, the computer cannot directly execute the human-readable source code. The source code must first be compiled into machine instructions and stored inside an ELF executable file.

In this project, the goal is to understand how a simple C program is transformed into an executable file and how the operating system loads and executes it. The investigation is performed using tools such as gcc, strip, readelf, ldd, and objdump. These tools allow us to examine the structure of the executable, understand its memory organization, and analyze how the program works internally.

# **1\. Compiling the C Program**

The first step is to compile the C source code into an executable file. This is done using the following command:

gcc \-Wall \-O0 \-fno-inline \-o program program.c

This command uses the GNU Compiler Collection (GCC) to translate the C source file (program.c) into an executable file called program.

The option \-Wall enables compiler warnings. Warnings help programmers identify possible problems in their code, such as unused variables, incorrect syntax, or programming mistakes. Although warnings do not always stop the compilation process, they help improve the reliability and quality of the program.

The option \-O0 disables compiler optimization. Normally, compilers optimize programs by changing the generated machine instructions to make them faster or smaller. However, optimization can make assembly analysis more difficult because the generated code may become very different from the original C program. Since this project involves studying the relationship between C code and assembly instructions, optimization is disabled so that the generated executable remains easier to understand.

The option \-fno-inline disables function inlining. Function inlining is a compiler optimization where the compiler replaces a function call with the actual function code. For example, instead of keeping a call to sumArray(), the compiler may directly insert the instructions of sumArray() into main(). This makes analysis harder because the original functions may disappear. Disabling this option ensures that the user-defined functions remain separate in the executable.

The option \-o program specifies the name of the output executable. Without this option, GCC normally creates a file called a.out. In this case, the executable is named program.

After compilation, the C program has been converted into an ELF executable that can be executed by the Linux operating system.

# **2\. Removing Debugging and Symbol Information**

After compiling the program, the next step is to remove unnecessary information using the command:

strip program

The strip command removes debugging information and symbol information from the executable file.

During compilation, the executable contains extra information that is useful for programmers and debugging tools. This information includes function names, variable names, and debugging symbols. For example, before stripping, the executable may contain names such as:

main()  
fillArray()  
sumArray()  
printResult()

After using strip, these names are removed, and the executable contains mostly machine instructions and memory addresses.

The purpose of stripping the executable in this project is to simulate a real-world reverse engineering situation. In many cases, programmers analyzing software do not have access to the original source code or function names. They must study the binary file by examining assembly instructions and the structure of the ELF file.

# **Part B: Static Analysis**

Static analysis means examining a program without executing it. Instead of running the program, we study its internal structure, memory organization, and machine instructions.

The main tools used for static analysis in this project are:

* readelf  
* objdump

These tools allow us to understand how the executable is organized and how the computer will execute it.

# **3\. Examining the ELF Header Using readelf**

The first static analysis command is:

readelf \-h program

The readelf command displays information stored inside the ELF header. The ELF header contains important details about the executable file, including the architecture, file type, and entry point.

One important piece of information is the executable architecture.

For example, the output may show:

Machine: Advanced Micro Devices X86-64

This means the executable was built for a 64-bit x86 processor architecture, which is commonly used by modern computers.

Another important value is the entry point address.

Example:

Entry point address: 0x1070

The entry point is the first memory address where execution begins when the operating system starts the program.

It is important to understand that the entry point is usually not the main() function. The operating system first starts a function called \_start. The execution flow is usually:

Operating System  
        |  
        v  
      \_start  
        |  
        v  
C Runtime Initialization  
        |  
        v  
       main()

The C runtime prepares the environment before transferring control to the programmer's main() function.

# **4\. Examining ELF Sections**

To examine the different sections inside the executable, we use:

readelf \-S program

An ELF executable is divided into different sections. Each section has a specific purpose and stores different types of information required for program execution.

## **The .text Section**

The .text section contains the executable machine instructions of the program.

This section includes:

* the main() function  
* user-defined functions  
* program logic

For example, a C statement such as:

printf("Hello World");

is converted into machine instructions, and those instructions are stored inside the .text section.

The .text section is normally marked as read-only because it contains instructions that should not be modified during execution.

## **The .data Section**

The .data section stores global and static variables that have an initial value.

For example:

int globalNumber \= 5;

Since this variable already has a value, it is stored inside the .data section.

The operating system loads this data into memory when the program starts.

## **The .bss Section**

The .bss section stores global and static variables that do not have an initial value.

Example:

int counter;

The variable exists in memory, but the program does not provide an initial value.

Instead of storing empty data inside the executable file, the operating system simply reserves space for these variables during program startup.

## **The .plt Section**

The .plt section stands for Procedure Linkage Table.

It is used when the program calls functions from external libraries.

Examples include:

printf()  
malloc()  
free()

These functions are not stored directly inside our executable. They come from shared libraries such as the C standard library (libc).

The PLT provides a connection between our program and these external functions.

## **The .got Section**

The .got section stands for Global Offset Table.

It stores addresses needed for accessing external functions and variables during dynamic linking.

For example, when the program calls:

printf()

the actual memory address of printf() is stored through the GOT after the shared library is loaded.

The GOT allows programs to use shared libraries while still being able to locate functions correctly in memory.

# **5\. Checking if the Program is Dynamically Linked**

To check how the executable uses external libraries, we run:

*ldd program*

The ldd command displays the shared libraries required by the executable.

Example output:

*libc.so.6*  
*linux-vdso.so*  
[*ld-linux-x86-64.so*](http://ld-linux-x86-64.so)

If libraries are displayed, the executable is dynamically linked.

Dynamic linking means that the program does not contain copies of all library functions inside itself. Instead, the operating system loads the required libraries when the program starts.

For example, our program uses:

*printf()*  
*malloc()*  
*free()*

These functions are provided by the C standard library (libc). During execution, the operating system loads this library and connects our program to the required functions.

Therefore, the executable is dynamically linked.

# **6\. Disassembling the Executable Using objdump**

The command:

*objdump \-d program*

is used to display the assembly instructions contained inside the executable.

Assembly language is a low-level representation of machine instructions that the processor executes.

For example, the original C code:

*sum \+= arr\[i\];*

may become several assembly instructions involving:

* loading values from memory  
* adding numbers  
* storing results

Because the executable has been stripped, the original function names may not appear. Instead of seeing:

*main:*  
*fillArray:*  
*sumArray:*

we may see only memory addresses.

To identify functions, we examine their behaviour.

For example:

* A function calling malloc is likely responsible for memory allocation.  
* A function calling printf is likely responsible for displaying output.  
* A function containing repeated comparison and jump instructions probably contains a loop.

# **Finding and Understanding main()**

After stripping, locating main() requires analysing the assembly.

The main() function can usually be identified by looking for instructions that:

* allocate memory  
* call user-defined functions  
* prepare program execution  
* call library functions

For example, if a section of assembly contains:

*call malloc*  
*call fillArray*  
*call sumArray*  
*call printResult*

this is likely the main function because it controls the overall program flow.

# **Conclusion**

The process of investigating an ELF executable allows us to understand how a simple C program becomes a machine-executable file. The compilation process converts source code into machine instructions, while stripping removes information that makes reverse engineering easier. Static analysis tools such as readelf, ldd, and objdump reveal important details about the executable, including its architecture, memory sections, linking method, and internal program logic.

By analysing ELF sections such as *.text, .data, .bss, .plt, and .got,* we can understand how instructions, variables, and external libraries are organized. Finally, using assembly analysis, we can reconstruct the behaviour of the original C program even after symbols and debugging information have been removed. This demonstrates how operating systems load, organize, and execute software at a low level.

