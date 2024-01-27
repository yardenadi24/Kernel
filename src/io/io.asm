section .asm

global insb
global insw
global outsb
global outsw

; Read one byte from the port
insb:
    push ebp
    mov ebp,esp
    
    xor eax,eax ; eax is allways the return value so we want to clean it
    mov edx, [ebp+8] ; move the first arg which is the byte to edx
    in al,dx ; insert dx value to al

    pop ebp
    ret

; Read one word from the port
insw:
    push ebp
    mov ebp,esp
    
    xor eax,eax ; eax is allways the return value so we want to clean it
    mov edx, [ebp+8] ; move the first arg which is the byte to edx
    in ax,dx ; insert dx value to ax

    pop ebp
    ret
; Write one word to the port
outsb:
    push ebp
    mov ebp,esp
    
    mov eax, [ebp+12] ; move the second arg which is the byte to edx
    mov edx, [ebp+8]  ; move the first arg which is the port to eax
    out dx,al ; insert dx value to al

    pop ebp
    ret
; Write one word to the port
outsw:
    push ebp
    mov ebp,esp
    
    mov eax, [ebp+12] ; move the second arg which is the word to edx
    mov edx, [ebp+8]  ; move the first arg which is the port to eax
    out dx,ax ; insert dx value to al

    pop ebp
    ret

