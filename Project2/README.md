# 

| Name | Link |
| :---- | :---- |
| **DemoVideo** | **[Project 2: Assembly-Based Text File Analysis](https://youtu.be/YtkWuiqToL0)**  |
|  |  |

# **Project 2: Assembly-Based Text File Analysis**

## **Explanation and Execution Guide**

## **Introduction**

Assembly language is a low-level programming language that allows direct communication with the computer processor. Unlike high-level languages such as C or Python, assembly programs do not use built-in functions for tasks such as opening files, reading data, or printing results. Instead, they communicate directly with the operating system using system calls.

This project implements an x86-64 Assembly program that reads a text file called sensor\_readings.txt and analyses its contents. The purpose of the program is to count the total number of records in the file and determine how many records contain valid data.

Each measurement in the file is stored on a separate line. Some lines may be empty, meaning that no sensor data was recorded. The program processes the file character by character, identifies line boundaries, checks whether each line contains data, and produces the final output:

*Total records: X*  
*Valid records: Y*

The program demonstrates important assembly concepts, including file handling, memory traversal, loops, conditional branches, and system calls.

# **Program Structure**

The assembly program is divided into three main sections:

* .data  
* .bss  
* .text

Each section has a specific role in storing information and executing instructions.

# **1\. Data Section (.data)**

The .data section stores initialized data that the program needs during execution.

The program stores the input filename:

*filename db "sensor\_readings.txt", 0*

This tells the operating system which file should be opened.

The program also stores the messages that will be displayed on the terminal:

*msg\_total db "Total records: "*  
*msg\_valid db "Valid records: "*

These messages are used when displaying the final results.

The newline character is also stored:

*newline db 10*

The value 10 represents the ASCII value of the newline character (\\n).

# **2\. BSS Section (.bss)**

The .bss section reserves memory that will be used while the program is running.

The file content is stored in a memory buffer:

*buffer resb 1024*

This creates space for 1024 bytes of file data.

The program uses two counters:

*total\_lines resq 1*  
*valid\_lines resq 1*

total\_lines stores the total number of lines found in the file.

valid\_lines stores the number of lines that contain actual sensor data.

The variable:

*has\_data resb 1*

acts as a flag. It tells the program whether the current line contains any characters.

For example:

*25*  
*30*

*45*

The empty third line will have:

*has\_data \= 0*

while the other lines will have:

*has\_data \= 1*

# **3\. Opening the File**

The first operation performed by the program is opening the file.

In high-level languages, this would normally be done using functions such as:

*fopen()*

However, assembly directly uses Linux system calls.

The instruction:

*mov rax, 2*

selects the Linux open system call.

The filename is passed to the operating system:

*mov rdi, filename*

The file is opened in read-only mode:

*mov rsi, 0*

After the system call:

*Syscall*

the operating system returns a file descriptor. This value is saved in the register r12 so it can be used later when reading the file.

# **4\. Reading the File**

After opening the file, the program loads the file contents into memory.

The program uses the Linux read system call:

*mov rax, 0*

The file descriptor is provided:

*mov rdi, r12*

The memory location where data will be stored is:

*mov rsi, buffer*

The maximum number of bytes to read is:

*mov rdx, 1024*

After reading, the number of bytes loaded is stored in:

*r13*

This value tells the program how much data it needs to analyse.

# **5\. Traversing the File Data**

The main part of the program is scanning the file contents.

The program moves through the buffer one character at a time.

The current position is stored in:

*rsi*

The remaining number of characters is stored in:

*rcx*

The loop continues until all characters have been checked.

The loop works as follows:

1. Read one character.  
2. Check whether it is a newline.  
3. If it is a newline, finish counting the current line.  
4. If it is not a newline, mark the line as containing data.  
5. Move to the next character.

This allows the program to analyse the file without needing to understand the actual sensor values.

# 

# **6\. Detecting Lines**

A line boundary is identified using the newline character:

*\\n*

The ASCII value of newline is:

*10*

The program checks:

*cmp al, 10*

If the character is a newline, the program jumps to the line-processing section.

When a newline is found:

*inc qword \[total\_lines\]*

increases the total number of records.

# **7\. Detecting Valid Records**

The program uses the variable:

*has\_data*

to determine whether the current line contains information.

At the beginning of a line:

*has\_data \= 0*

If the program finds a normal character, it changes the value:

*has\_data \= 1*

When the program reaches the end of the line, it checks this value.

If:

*has\_data \= 1*

the record is valid:

*inc qword \[valid\_lines\]*

If:

*has\_data \= 0*

the line is empty and is ignored.

# **8\. Displaying Results**

After processing the complete file, the program prints the results.

The output messages are displayed using the Linux write system call.

Example:

*Total records: 5*  
*Valid records: 4*

Because assembly works with numeric values instead of text, the program converts the numbers into ASCII characters before displaying them.

For example:

The internal number:

*5*

is converted into:

*'5'*

so that it can be printed on the screen.

# **9\. Program Termination**

After displaying the results, the program exits using the Linux exit system call:

*mov rax, 60*  
*syscall*

This informs the operating system that the program has finished successfully.

# **How to Run the Program**

## **1\. Create the input file**

Create a file named:

*sensor\_readings.txt*

Example:

*25*  
*30*

*45*  
*50*

Save it in the same folder as the assembly program.

## **2\. Install NASM**

If NASM is not installed:

*sudo apt install nasm*

NASM is the assembler used to convert Assembly code into machine code.

## **3\. Assemble the Program**

Run:

*nasm \-f elf64 sensor\_analysis.asm*

This converts the assembly source file into an object file:

*sensor\_analysis.o*

## **4\. Link the Object File**

Run:

*ld sensor\_analysis.o \-o sensor\_analysis*

This creates the final executable:

*sensor\_analysis*

## **5\. Execute the Program**

Run:

*./sensor\_analysis*

Example output:

*Total records: 5*  
*Valid records: 4*

# **Conclusion**

This Assembly program demonstrates how low-level software interacts directly with the operating system. Instead of relying on high-level file handling functions, the program uses system calls to open and read files. It then processes the data stored in memory by manually checking each character and using loops and conditional branches.

The project demonstrates fundamental assembly programming concepts, including file operations, memory management, control flow, and data processing. By analysing a text file at the character level, the program shows how simple tasks performed in high-level languages are implemented internally at the processor level.

