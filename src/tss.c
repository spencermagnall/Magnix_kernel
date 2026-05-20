#include "tss.h"
#include "gdt.h"
#include "serial.h"

static tss_t tss; 

void tss_init(void){

    // Set the whole struct up with zeros instead of garbage
    // will be filled in later

    for (uint64_t i = 0; i < sizeof(tss_t); i++) {
        ((uint8_t *)&tss)[i] = 0;
    }

    // Setting this to tss + 1 byte disables I/O for ring 3 for 
    // some reason
    tss.iopb_offset = (uint16_t)sizeof(tss_t);
    
    // Setup base and limit 
    uint64_t base = (uint64_t) &tss; 
    uint32_t limit = (uint32_t) sizeof(tss) - 1;

    print_serial("before gdt_set_tss\n");
    // Make the TSS entry in the GDT
    gdt_set_tss(base, limit);
    print_serial("after gdt_set_tss\n");
    print_serial("before ltr\n");

        // load it into the reg 
    __asm__ volatile("ltr %0" : : "r"((uint16_t)GDT_TSS));
    print_serial("after ltr\n");

}