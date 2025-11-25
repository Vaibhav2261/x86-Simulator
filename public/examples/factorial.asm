mov eax, 6
mov ebx, 1

factorial_loop:
imul ebx, eax
dec eax
cmp eax, 1
jg factorial_loop

print ebx
hlt