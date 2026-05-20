global gdt_load
 
gdt_load:
    lgdt [rdi]          ; rdi = pointer to gdtr (System V ABI first arg)
 
    ; reload data segments
    mov ax, 0x10        ; GDT_KERNEL_DATA
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
 
    ; reload CS via a far return — you can't mov directly into CS
    ; retfq pops RIP first, then CS, so we arrange the stack accordingly
    pop rdi             ; stash our return address
    push 0x08           ; GDT_KERNEL_CODE
    push rdi            ; return address back on top
    retfq               ; far return: jumps to return address with CS = 0x08
