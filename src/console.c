#include "console.h"


static Console con;

static PSF_font *default_font = (PSF_font *)&_binary_font_psf_start; // a bit sloppy, should this just be extern as we're importing psf.h?

void console_init(Framebuffer *fb){

    con.fb = *fb;
    con.width = fb->width / default_font->width;
    con.height = fb->height / default_font->height;
    con.cx = 0;
    con.cy = 0;
    con.fg = FG;
    con.bg = BG;

}

void print_char(char c){

    // If \n set console cursor y=y+1 and set x=0
    if (c == '\n') {
        con.cx = 0;
        con.cy++;
        return;
    }

    // if \r set x=0 
    if (c == '\r') {
        con.cx = 0;
        return;
    } 

    // if \t - fancy tab stuff
    if (c == '\t') {
        con.cx = (con.cx + 8) & ~7; // round up to next multiple of 8
        if (con.cx >= con.width) {
            con.cx = 0;
            con.cy++;
        }
        return;
    }

    // otherwise call fb_putchar
    fb_putchar(con.fb.addr, c, con.cx, con.cy, con.fg,  con.bg, con.fb.pitch);

    con.cx++; // <-- the culprit - next time don't be a dumbass 
    if (con.cx >= con.width) {
        con.cx = 0;
        con.cy++;
    }

}

void print_string(char *str){

    while(*str){
        // Magic!
        // *str++ increments string and returns the old 
        // derefernced version of str (e.g a char)
        print_char(*str++);
    }

}