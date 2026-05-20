#include "gdt.h"

static gdt_entry_t gdt[7]; // NULL, kernelcode, kerneldata, userdata, usercode, TSS[6,7]
static gdtr_t      gdtr;

#define GDT_PRESENT     (1 << 7)  // segment is valid
#define GDT_DPL_RING0   (0 << 5)  // kernel privilege
#define GDT_DPL_RING3   (3 << 5)  // user privilege
#define GDT_NONSYSTEM   (1 << 4)  // code or data segment (not TSS/LDT)
#define GDT_EXECUTABLE  (1 << 3)  // code segment
#define GDT_READWRITE   (1 << 1)  // readable (code) / writable (data)
#define GDT_TSS_AVAILABLE  0x09  // 64-bit TSS, available (not busy)
 
#define GDT_ACCESS_KERNEL_CODE  (GDT_PRESENT | GDT_DPL_RING0 | GDT_NONSYSTEM | GDT_EXECUTABLE | GDT_READWRITE)  // 0x9A
#define GDT_ACCESS_KERNEL_DATA  (GDT_PRESENT | GDT_DPL_RING0 | GDT_NONSYSTEM | GDT_READWRITE)                   // 0x92
#define GDT_ACCESS_USER_CODE    (GDT_PRESENT | GDT_DPL_RING3 | GDT_NONSYSTEM | GDT_EXECUTABLE | GDT_READWRITE)  // 0xFA
#define GDT_ACCESS_USER_DATA    (GDT_PRESENT | GDT_DPL_RING3 | GDT_NONSYSTEM | GDT_READWRITE)                   // 0xF2
#define GDT_ACCESS_TSS  (GDT_PRESENT | GDT_TSS_AVAILABLE)  // present, ring 0, 64-bit TSS available


// ─── flag nibble constants ────────────────────────────────────────────────────
 
#define GDT_FLAG_GRANULARITY  (1 << 3)  // limit in 4KB pages
#define GDT_FLAG_32BIT        (1 << 2)  // 32-bit protected mode
#define GDT_FLAG_64BIT        (1 << 1)  // 64-bit long mode (L bit)
 
// code64: L=1, G=1, DB must be 0 (L and DB are mutually exclusive)
// data32: G=1, DB=1 (L bit ignored for data in 64-bit mode)
#define GDT_FLAGS_CODE64  (GDT_FLAG_GRANULARITY | GDT_FLAG_64BIT)
#define GDT_FLAGS_DATA32  (GDT_FLAG_GRANULARITY | GDT_FLAG_32BIT)

// Make gdt entry helper function so you don't get more of a headache for this book keeping

static gdt_entry_t make_entry(uint32_t base, uint32_t limit, uint8_t access, uint8_t flags) {
    gdt_entry_t e;
    e.limit_low        = (uint16_t)(limit & 0xFFFF);
    e.base_low         = (uint16_t)(base  & 0xFFFF);
    e.base_mid         = (uint8_t)((base  >> 16) & 0xFF);
    e.access           = access;
    // low nibble = limit bits 19:16, high nibble = flags
    e.limit_high_flags = (uint8_t)((limit >> 16) & 0x0F) | ((flags & 0x0F) << 4);
    e.base_high        = (uint8_t)((base  >> 24) & 0xFF);
    return e;
}

void gdt_set_tss(uint64_t base, uint32_t limit){
    // Upper 32 bits of the TSS address the rest are in the next byte
    // Set flag as 0 for no granularity - limit is in bytes 
    gdt[5] = make_entry((uint32_t)(base & 0xFFFFFFFF), limit, GDT_ACCESS_TSS, 0);


    // Lower 32 bits 
    // Most of this is reserved 
    // slot 6 is raw — upper 32 bits of base at bytes 0-3, rest must be zero
    uint32_t *slot6 = (uint32_t *)&gdt[6];
    slot6[0] = (uint32_t)(base >> 32);
    slot6[1] = 0;

}

void gdt_init(void) {
    gdt[0] = make_entry(0, 0,        0,                      0);                // null descriptor (required)
    gdt[1] = make_entry(0, 0xFFFFF,  GDT_ACCESS_KERNEL_CODE, GDT_FLAGS_CODE64); // kernel code
    gdt[2] = make_entry(0, 0xFFFFF,  GDT_ACCESS_KERNEL_DATA, GDT_FLAGS_DATA32); // kernel data
    gdt[3] = make_entry(0, 0xFFFFF,  GDT_ACCESS_USER_CODE,   GDT_FLAGS_CODE64); // user code
    gdt[4] = make_entry(0, 0xFFFFF,  GDT_ACCESS_USER_DATA,   GDT_FLAGS_DATA32); // user data
 
    gdtr.limit = (uint16_t)(sizeof(gdt) - 1);
    gdtr.base  = (uint64_t)&gdt;
 
    gdt_load(&gdtr);
}
