section .asm

; --- STACK ---
; --- ebp ---- ebp
; --- ebp+4--- Return address
; --- ebp+8--- Arg 1
; --- ebp+12-- Arg 2
; --- .... ---

extern int21h_handler
extern no_interrupt_handler
global int21h
global idt_load
global no_interrupt

idt_load:
    push ebp
    mov ebp, esp

    mov ebx, [ebp+8] ; Points to the first arg that was provided by the function
    lidt [ebx] ; Loads iterrupt descriptors table
    
    pop ebp
    ret

int21h:
    cli ; Clear interrupt
    pushad ; Push all general registers
    call int21h_handler
    popad  ; Pop all general registers
    sti ; Start interrupts
    iret

no_interrupt:
    cli ; Clear interrupt
    pushad ; Push all general registers
    call no_interrupt_handler
    popad  ; Pop all general registers
    sti ; Start interrupts
    iret