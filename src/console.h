#ifndef CONSOLE
#define CONSOLE

#include<stdint.h>
#include "serial.h"
#include "psf.h"


#define FG 0xFFFFFFFF
#define BG 0x00000000


typedef struct {
    volatile uint32_t *addr;
    uint32_t pitch;
    uint32_t width;  // in pixels
    uint32_t height; // in pixels
} Framebuffer;

typedef struct {
    Framebuffer fb;
    uint16_t width;  // in characters
    uint16_t height; // in characters
    uint16_t cx;
    uint16_t cy;
    uint32_t fg;
    uint32_t bg;
} Console;


void console_init(Framebuffer *fb);

void print_char(char c);
void print_string(char *str); 

#endif // CONSOLE 