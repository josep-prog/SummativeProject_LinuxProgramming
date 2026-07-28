section .data

filename db "sensor_readings.txt", 0

msg_total db "Total records: "
len_total equ $-msg_total

msg_valid db "Valid records: "
len_valid equ $-msg_valid

newline db 10

err_open_msg db "Error: cannot open input file", 10
err_open_len equ $-err_open_msg

err_read_msg db "Error: failed to read input file", 10
err_read_len equ $-err_read_msg


section .bss

buffer resb 1024          ; memory space for file content

total_lines resq 1        ; store total number of lines
valid_lines resq 1        ; store number of non-empty lines

has_data resb 1           ; flag to check if current line has data

number_buffer resb 20     ; temporary space for printing numbers


section .text

global _start


_start:

; Open file

mov rax, 2                ; sys_open
mov rdi, filename         ; file name
mov rsi, 0                ; read-only mode
syscall

mov r12, rax              ; save file descriptor

cmp rax, 0                ; a negative return value means open() failed
jl open_error


; Read file into memory

mov rax, 0                ; sys_read
mov rdi, r12              ; file descriptor
mov rsi, buffer           ; store data here
mov rdx, 1024             ; maximum bytes to read
syscall

mov r13, rax              ; save number of bytes read

cmp rax, 0                ; a negative return value means read() failed
jl read_error


; Initialize counters

mov qword [total_lines], 0
mov qword [valid_lines], 0
mov byte [has_data], 0


; Check every character in the file

mov rsi, buffer
mov rcx, r13


loop_chars:

cmp rcx, 0
je finish                 ; stop when no characters remain


mov al, [rsi]             ; get current character


; Check if character is newline

cmp al, 10                ; ASCII value of '\n'
je new_line


; Check if character is carriage return (CRLF line ending)

cmp al, 13                ; ASCII value of '\r'
je next_character         ; ignore CR itself; matching '\n' still follows


; Normal character means line has data

mov byte [has_data], 1

jmp next_character



new_line:

inc qword [total_lines]   ; count this line


; Count valid line if it had data

cmp byte [has_data], 1
jne empty_line

inc qword [valid_lines]


empty_line:

mov byte [has_data], 0   ; reset for next line



next_character:

inc rsi                   ; move to next character
dec rcx                   ; decrease remaining characters

jmp loop_chars



finish:

; Handle final line if file does not end with a newline
cmp byte [has_data], 1
jne finish_print

inc qword [total_lines]
inc qword [valid_lines]


finish_print:

; Print total records message

mov rax, 1                ; sys_write
mov rdi, 1                ; terminal output
mov rsi, msg_total
mov rdx, len_total
syscall


; Print total number

mov rax, [total_lines]
call print_number



; Print newline

mov rax, 1
mov rdi, 1
mov rsi, newline
mov rdx, 1
syscall



; Print valid records message

mov rax, 1
mov rdi, 1
mov rsi, msg_valid
mov rdx, len_valid
syscall


; Print valid number

mov rax, [valid_lines]
call print_number



; Exit program

mov rax, 60               ; sys_exit
mov rdi, 0
syscall



; File could not be opened (e.g. missing file, no permission):
; report the error on stderr and exit with a non-zero status.

open_error:

mov rax, 1                ; sys_write
mov rdi, 2                ; stderr
mov rsi, err_open_msg
mov rdx, err_open_len
syscall

mov rax, 60                ; sys_exit
mov rdi, 1                  ; non-zero exit status signals failure
syscall



; read() returned an error after the file was successfully opened:
; report the error on stderr and exit with a non-zero status.

read_error:

mov rax, 1
mov rdi, 2
mov rsi, err_read_msg
mov rdx, err_read_len
syscall

mov rax, 60
mov rdi, 1
syscall



; Convert number to text and print it

print_number:

mov rbx, 10
mov rdi, number_buffer+20


convert:

xor rdx, rdx
div rbx

add dl, '0'

dec rdi
mov [rdi], dl


cmp rax, 0
jne convert



; Print converted number

mov rax, 1
mov rsi, rdi
mov rdx, number_buffer+20
sub rdx, rdi
mov rdi, 1

syscall

ret
