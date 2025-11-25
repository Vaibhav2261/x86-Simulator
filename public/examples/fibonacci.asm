mov ecx, 10
mov eax, 0
mov ebx, 1

loop_start:
print eax
mov al, 10
printc al

mov edx, eax
add eax, ebx
mov ebx, edx

dec ecx
cmp ecx, 0
jg loop_start

hlt