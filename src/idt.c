#include "idt.h"

static idt_entry_t idt[256]; // For now just vector 0 
static idtr_t idtr; 

extern void isr_0(void);


#define GDT_KERNEL_CODE  0x08   // entry 1, ring 0
#define IDT_INTERUPT_GATE 0x8E // (p=1, dpl=0b00, type=0b1110 => type_attributes=0b1000_1110=0x8E)

static idt_entry_t make_entry(void *isr, uint8_t  ist) {
    idt_entry_t e;
    uint64_t offset = (uint64_t) isr; 
    e.offset_1 = (uint16_t)(offset & 0xFFFF);        // offset bits 0..15
    e.selector = GDT_KERNEL_CODE;        // a code segment selector in GDT or LDT
    e.ist = ist;             // bits 0..2 holds Interrupt Stack Table offset, rest of bits zero.
    e.type_attributes = IDT_INTERUPT_GATE; // gate type, dpl, and p fields
    e.offset_2 = (uint16_t)((offset >> 16) & 0xFFFF);        // offset bits 16..31
    e.offset_3 = (uint32_t)(offset >> 32);        // offset bits 32..63
    e.zero = 0;            // reserved
    return e;
}

void idt_init(){

    idt[0] = make_entry(isr_0, 0);

    idtr.limit = (uint16_t)(sizeof(idt) - 1); 
    idtr.base = (uint64_t)&idt;

    idt_load(&idtr);




}

void interrupt_handler(){
    print_serial("kernel panic!!!");
    for(;;) __asm__ volatile("hlt");
}