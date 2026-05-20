global idt_load
global isr_0 
extern interrupt_handler

isr_0:
    push 0          ; dummy error code
    push 0          ; vector number

isr_common:
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

    mov rdi, rsp
    call interrupt_handler

    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rsi
    pop rdi
    pop rdx
    pop rcx
    pop rbx
    pop rax

    add rsp, 16     ; pop vector and error code
    iretq

idt_load:
    lidt [rdi]
    ret 