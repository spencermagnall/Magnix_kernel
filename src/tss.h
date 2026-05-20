#ifndef TSS
#define TSS
#include <stdint.h> 


// typedef struct  {
//     uint32_t reserved0;
//     // rsp regs
//     uint64_t rsp0;
//     uint64_t rsp1;
//     uint64_t rsp2;
    
//     // reserved 
//     uint32_t reserved1;
//     uint32_t reserved2;
    
//     // Interrupt stack table(s) 
//     uint64_t ist1;
//     uint64_t ist2; 
//     uint64_t ist3;
//     uint64_t ist4;
//     uint64_t ist5; 
//     uint64_t ist6; 
//     uint64_t ist7;

//     // reserved
//     uint32_t reserved3;
//     uint32_t reserved4; 

//     uint16_t reserved5;
//     // IOPB- I/O Map Base Address Field.
//     uint16_t iopb_offset;


// } __attribute__((packed)) tss_t; 

typedef struct {
    uint32_t reserved0;         // 0x00

    uint32_t rsp0_low;          // 0x04
    uint32_t rsp0_high;         // 0x08
    uint32_t rsp1_low;          // 0x0C
    uint32_t rsp1_high;         // 0x10
    uint32_t rsp2_low;          // 0x14
    uint32_t rsp2_high;         // 0x18

    uint32_t reserved1;         // 0x1C
    uint32_t reserved2;         // 0x20

    uint32_t ist1_low;          // 0x24
    uint32_t ist1_high;         // 0x28
    uint32_t ist2_low;          // 0x2C
    uint32_t ist2_high;         // 0x30
    uint32_t ist3_low;          // 0x34
    uint32_t ist3_high;         // 0x38
    uint32_t ist4_low;          // 0x3C
    uint32_t ist4_high;         // 0x40
    uint32_t ist5_low;          // 0x44
    uint32_t ist5_high;         // 0x48
    uint32_t ist6_low;          // 0x4C
    uint32_t ist6_high;         // 0x50
    uint32_t ist7_low;          // 0x54
    uint32_t ist7_high;         // 0x58

    uint32_t reserved3;         // 0x5C
    uint32_t reserved4;         // 0x60

    uint16_t reserved5;         // 0x64
    uint16_t iopb_offset;       // 0x66
} __attribute__((packed)) tss_t;

void tss_init(void);

#endif // TSS