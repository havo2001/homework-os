.intel_syntax noprefix
.global sort
.text
# Bubble sort using two for loops
sort:
	# Prologue:
	push ebp
	mov ebp, esp
	sub esp, 4
        push ebx
	
	# ebx store index i	
	mov ebx, 0
	

loop1: 
	# ecx store index j, loop1 is outer loop
	mov ecx, ebx
	inc ebx
	cmp ebx, [ebp + 12]
	jae end

	mov esi, [ebp + 16]
	imul esi, ecx
	add esi, [ebp + 8]
	jmp loop2
	
loop2:	
        # loop 2 is inner loop
	inc ecx
	cmp ecx, [ebp + 12]
	jae loop1

 	mov edi, [ebp + 16]
	imul edi, ecx
	add edi, [ebp + 8]
	push ebx
	push ecx
	
	# compare buffer[i] and buffer[j]
	push edi
	push esi
	call [ebp + 20]
	add esp, 8
	
	pop ecx
	pop ebx
	cmp eax, 1 
	je swap

	jmp loop2


swap:
	push ebx
	push ecx
 
	# calling malloc
	mov edx, [ebp + 16]
	push edx
	call malloc
	add esp, 4
	mov [ebp - 4], eax

	# calling memcpy 
	mov edx, [ebp + 16]
	push edx
	mov edx, edi
	push edx
	mov edx, [ebp - 4]
	push edx
	call memcpy
	add esp, 12

	# calling memcpy
	mov edx, [ebp + 16]
	push edx
	mov edx, esi
	push edx
	mov edx, edi
	push edx
	call memcpy
	add esp, 12
	
	# calling memcpy
	mov edx, [ebp + 16]
	push edx
	mov edx, [ebp - 4]
	push edx
	mov edx, esi
	push edx
	call memcpy
	add esp, 12
	
	# calling free
	mov edx, [ebp - 4]
	push edx
	call free
	add esp, 4
	
	pop ecx
	pop ebx	
	jmp loop2



end:	
	pop ebx
	mov esp, ebp
	pop ebp
	ret
	
