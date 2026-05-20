#ifndef GDT
#define GDT


#include <stdint.h>


// Alot of this is just copy paste
// Doesn't seem to be much freedom here 
// because of how strict the CPU is 

 // These are selector segments - 16bit 
 // https://wiki.osdev.org/Segment_Selector 
 // In our case just ring 0 and ring 3 
 // two lowest bits are the RPL 
#define GDT_KERNEL_CODE  0x08   // entry 1, ring 0
#define GDT_KERNEL_DATA  0x10   // entry 2, ring 0
#define GDT_USER_CODE    0x1B   // entry 3, ring 3  (0x18 | 3)
#define GDT_USER_DATA    0x23   // entry 4, ring 3  (0x20 | 3)
#define GDT_TSS          0x28   // entries 5 and 6 ring 0, 16 byte

// Struct for a GDT entry - 8 bytes
typedef struct {
    uint16_t limit_low;    // bits 15:0 of limit
    uint16_t base_low;     // bits 15:0 of base
    uint8_t  base_mid;     // bits 23:16 of base
    uint8_t  access;       // the access byte (present, DPL, type, etc.)
    uint8_t  limit_high_flags; // bits 19:16 of limit in low nibble, flags in high nibble
    uint8_t  base_high;    // bits 31:24 of base
} __attribute__((packed)) gdt_entry_t; // packed to stop the compiler changing the size with padding 

// We literally load this into a register to tell the CPU where the GDT is 
typedef struct {
    uint16_t limit;   // size of GDT in bytes, minus 1
    uint64_t base;    // linear (virtual) address of the GDT array
} __attribute__((packed)) gdtr_t;


// loads the GDTR, reloads segment registers, reloads CS.
// x86_64 magic that I dont understand
extern void gdt_load(gdtr_t *gdtr);
 
// Populate the GDT entries and call gdt_load. Call this before idt_init().
void gdt_init(void);

// Setup the TSS entry in slots 5 and 6 
void gdt_set_tss(uint64_t base, uint32_t limit);

#endif // GDT 