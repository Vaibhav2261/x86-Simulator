; Advanced test program showcasing various instructions

; Fibonacci with function call
mov ecx, 8
call fibonacci
print eax
mov al, 10
printc al

; Factorial
mov eax, 5
call factorial
print ebx
mov al, 10
printc al

hlt

; Fibonacci subroutine
fibonacci:
    push ebx
    push edx
    
    mov eax, 0
    mov ebx, 1
    
fib_loop:
    mov edx, eax
    add eax, ebx
    mov ebx, edx
    dec ecx
    cmp ecx, 0
    jg fib_loop
    
    pop edx
    pop ebx
    ret

; Factorial subroutine
factorial:
    push ecx
    mov ebx, 1
    
fact_loop:
    imul ebx, eax
    dec eax
    cmp eax, 1
    jg fact_loop
    
    pop ecx
    ret