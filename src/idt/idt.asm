section .asm

; --- STACK ---
; --- ebp ---- ebp
; --- ebp+4--- Return address
; --- ebp+8--- Arg 1
; --- ebp+12-- Arg 2
; --- .... ---

global idt_load
idt_load:
    push ebp
    mov ebp, esp

    mov ebx, [ebp+8] ; Points to the first arg that was provided by the function
    lidt [ebx] ; Loads iterrupt descriptors table
    
    pop ebp
    ret