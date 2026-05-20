#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <limine.h>
#include "gdt.h"
#include "serial.h"
#include "psf.h"
#include "tss.h"
#include "console.h"




// Set the base revision to 6, this is recommended as this is the latest
// base revision described by the Limine boot protocol specification.
// See specification for further info.

// This is a GCC extension. The used keyword tells the complier to keep it the object file 
// in the section called ".limine_requests"
__attribute__((used, section(".limine_requests")))
// static restricts the scope to this file, volatile forces the cpu to read it fresh each time 
// without caching. 
static volatile uint64_t limine_base_revision[] = LIMINE_BASE_REVISION(6);

// The Limine requests can be placed anywhere, but it is important that
// the compiler does not optimise them away, so, usually, they should
// be made volatile or equivalent, _and_ they should be accessed at least
// once or marked as used with the "used" attribute as done here.

__attribute__((used, section(".limine_requests")))
// Dot notation here is initalisng the struct variables, while we make the struct. 
// e.g we are making a framebuffer struck with id and revision componets and then filling it 
// types are probably encoded in the header file ( limine.h)
static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST_ID,
    .revision = 0
};



// Finally, define the start and end markers for the Limine requests.
// These can also be moved anywhere, to any .c file, as seen fit.

// No idea what these do! Should read up on it...

__attribute__((used, section(".limine_requests_start")))
static volatile uint64_t limine_requests_start_marker[] = LIMINE_REQUESTS_START_MARKER;

__attribute__((used, section(".limine_requests_end")))
static volatile uint64_t limine_requests_end_marker[] = LIMINE_REQUESTS_END_MARKER;

// GCC and Clang reserve the right to generate calls to the following
// 4 functions even if they are not directly called.
// Implement them as the C specification mandates.
// DO NOT remove or rename these functions, or stuff will eventually break!
// They CAN be moved to a different .c file.

void *memcpy(void *restrict dest, const void *restrict src, size_t n) {
    uint8_t *restrict pdest = dest;
    const uint8_t *restrict psrc = src;

    for (size_t i = 0; i < n; i++) {
        pdest[i] = psrc[i];
    }

    return dest;
}

void *memset(void *s, int c, size_t n) {
    uint8_t *p = s;

    for (size_t i = 0; i < n; i++) {
        p[i] = (uint8_t)c;
    }

    return s;
}

void *memmove(void *dest, const void *src, size_t n) {
    uint8_t *pdest = dest;
    const uint8_t *psrc = src;

    if ((uintptr_t)src > (uintptr_t)dest) {
        for (size_t i = 0; i < n; i++) {
            pdest[i] = psrc[i];
        }
    } else if ((uintptr_t)src < (uintptr_t)dest) {
        for (size_t i = n; i > 0; i--) {
            pdest[i-1] = psrc[i-1];
        }
    }

    return dest;
}

int memcmp(const void *s1, const void *s2, size_t n) {
    const uint8_t *p1 = s1;
    const uint8_t *p2 = s2;

    for (size_t i = 0; i < n; i++) {
        if (p1[i] != p2[i]) {
            return p1[i] < p2[i] ? -1 : 1;
        }
    }

    return 0;
}

// Halt and catch fire function.
// https://en.wikipedia.org/wiki/Halt_and_Catch_Fire_(computing)
static void hcf(void) {
    // while(1) but no compiler errors or warnings 
    for (;;) {
        asm ("hlt");
    }
}

// Helper function to print hex over the COM1
void print_hex_byte(uint8_t byte) {
    char hex[] = "0123456789ABCDEF";
    write_serial(hex[byte >> 4]);
    write_serial(hex[byte & 0x0F]);
    write_serial(' ');
}

// The following will be our kernel's entry point.
// If renaming kmain() to something else, make sure to change the
// linker script accordingly.
void kmain(void) {
    // Ensure the bootloader actually understands our base revision (see spec).
    if (LIMINE_BASE_REVISION_SUPPORTED(limine_base_revision) == false) {
        hcf();
    }

    // Ensure we got a framebuffer.
    if (framebuffer_request.response == NULL
     || framebuffer_request.response->framebuffer_count < 1) {
        hcf();
    }

    // Fetch the first framebuffer.
    struct limine_framebuffer *framebuffer = framebuffer_request.response->framebuffers[0];

    // Print a nice pattern to screen as an example.
    // Note: we assume the framebuffer model is RGB with 32-bit pixels.
    volatile uint32_t *fb_ptr = framebuffer->address;
    uint32_t pitch = framebuffer->pitch; 

    print_serial("before gdt_init\n");
    gdt_init();
    print_serial("after gdt_init\n");
    print_serial("Before tss_init\n");
    tss_init();
    print_serial("After tss init\n");

    init_serial();
    print_serial("Hello from the kernel!");

    uint8_t *raw = (uint8_t *)_binary_font_psf_start;
    for (int i = 0; i < 4; i++) {
        print_hex_byte(raw[i]);
    }
    write_serial('\n');

    PSF_font *default_font = (PSF_font *)&_binary_font_psf_start;
    // Check magic before fb_putchar
    if (default_font->magic == PSF_FONT_MAGIC) {
        print_serial("PSF2 font detected");
    } else if (default_font->magic == PSF1_FONT_MAGIC){
        print_serial("PSF1 FONT detected!");
    }

    // // Draw the 'A' char on the screen using the framebuffer and the psf
    // fb_putchar(fb_ptr, 'A', 0, 0,0xFFFFFFFF, 0x00000000, pitch);

    // Setup console_fb
    Framebuffer console_fb =  {
    .addr = fb_ptr,
    .pitch = pitch,
    .width = framebuffer->width,  // in pixels
    .height = framebuffer->height // in pixels
    };
    // Setup the console and print some text
    console_init(&console_fb);

    print_string("Hello from the kernel! \n");
    print_string("Hello on a new line!!!!"); 

    // for (size_t y = 0; y < framebuffer->height; y++) {
    //     for (size_t x = 0; x < framebuffer->width; x++) {
    //         uint32_t nX = x * 255 / framebuffer->width;
    //         uint32_t nY = y * 255 / framebuffer->height;
    //         fb_ptr[y * (framebuffer->pitch / 4) + x] = (nY << 8) | nX;
    //     }
    // }

    // Setup serial port 
    init_serial();

    // Hello from the kernel!
    //write_serial('a');
    //char mystring[] = "Hello from the Kernel!";
    print_serial("Hello from the kernel!");
    // We're done, just hang...
    hcf();
}