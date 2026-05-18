#include "serial.h"

// Serial port sends things one byte at a time

// First Com port - technically this is 0x03F8 if you get confused
// so 2 bytes = 16 bit 
#define PORT 0x3F8 
// This is just for clarity - Transmitter Holding Register Empty: Safe to write data!
#define STATUS_THRE 0x20

static inline void outb(uint16_t port, uint8_t val) {
    asm volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) : "memory");
}

static inline uint8_t inb(uint16_t port){
    uint8_t ret; 
    asm volatile ( "inb %w1, %b0"
                   : "=a"(ret)
                   : "Nd"(port)
                   : "memory");
    return ret; 
}


// Rx data 
int serial_received() {
   return inb(PORT + 5) & 1; // port + 5 is the line status register; has anything arrived? 
}

char read_serial() {
   while (serial_received() == 0); // while something has arrived 

   return inb(PORT); // return the char as that byte
}

// Tx data
int is_transmit_empty() {
   return inb(PORT + 5) & STATUS_THRE; // reg empty?
}

void write_serial(char a) {
   while (is_transmit_empty() == 0);

   outb(PORT,a);
}

// Print serial
void print_serial(char myString[]){ 
   for (int i=0; myString[i] != '\0'; i++){
        // Write out the string one char(byte) at a time
         write_serial(myString[i]);
   }
}


int init_serial() {

   // No idea what any of this shit does, have a look at the OSdev wiki for details
   outb(PORT + 1, 0x00);    // Disable all interrupts
   outb(PORT + 3, 0x80);    // Enable DLAB (set baud rate divisor)
   outb(PORT + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
   outb(PORT + 1, 0x00);    //                  (hi byte)
   outb(PORT + 3, 0x03);    // 8 bits, no parity, one stop bit
   outb(PORT + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
   outb(PORT + 4, 0x0B);    // IRQs enabled, RTS/DSR set
   outb(PORT + 4, 0x1E);    // Set in loopback mode, test the serial chip
   outb(PORT + 0, 0xAE);    // Test serial chip (send byte 0xAE and check if serial returns same byte)

   // Check if serial is faulty (i.e: not same byte as sent)
   if(inb(PORT + 0) != 0xAE) {
      return 1;
   }

   // If serial is not faulty set it in normal operation mode
   // (not-loopback with IRQs enabled and OUT#1 and OUT#2 bits enabled)
   outb(PORT + 4, 0x0F);
   return 0;
}



