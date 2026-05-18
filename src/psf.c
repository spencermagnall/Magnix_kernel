#include "psf.h"


/* import the font contained in the object file created above */
PSF_font *default_font = (PSF_font *)&_binary_font_psf_start;

/*TODO PSF1 file format*/
// Note that this will also mean putchar will need to be rewritten to be more flexible
// uint8_t* get_glyph_v1(uint8_t i){
//     // Code for PSF1 format
//     // Note that for PSF1 gylph width is always one byte (total glyph offest is width x height)
//     uint8_t *selected_glyph_v1 = (uint8_t*) &_binary_font_psf_start +
//     sizeof(PSF1_Header) + (i * default_font->characterSize);

//     return selected_glyph_v1;
// }

uint8_t* get_glyph_v2(uint8_t i){
    // PSF2 format
    // Note that widths in psf2 can be whatever, hence the dicking about
    uint8_t* selected_glyph_v2 = (uint8_t*) &_binary_font_psf_start +
    default_font->headersize + (i * default_font->bytesperglyph);

    return selected_glyph_v2;
}

void fb_putchar(volatile uint32_t *fb_ptr, char symbol, uint16_t cx, uint16_t cy, uint32_t fg, uint32_t bg,uint32_t pitch){

    // Step 1 - read the glyph stored in the position given by symbol

    // Not sure how I know which to use at run time so for now we use v2
    uint8_t *currentGylph = get_glyph_v2(symbol);

    // Step 2 - draw it at row cx and column cy (don't forget they are "character" coordinates)
    // I'll need to get the limine framebuffer first
    // Given cx and cy , work out where to start in pixel space (i.e calculate the offset) this is the top
    // lh corner of the glyph
   // Remember we are bitmapped 1bit = 1 pixel
    uint32_t pixel_x = cx * default_font->width;
    uint32_t pixel_y = cy * default_font->height;

    //How many 4-byte pixel slots make up one full horizontal line in RAM?
    uint32_t slots_per_row = pitch / 4;

    uint32_t offs = (pixel_y * slots_per_row) + pixel_x;
    uint32_t bytesPerGlyphLine = (default_font->width + 7) / 8;

 
    for (uint32_t y =0; y < default_font->height; y++){
        // Get the row for this glyph, i.e we offset the glyph in y to look at whatever row we are on
        uint8_t *currentByte = (y*bytesPerGlyphLine) + currentGylph;
        /* display a row */
        for(uint32_t x = 0; x <default_font->width; x++){

         
            uint8_t font_byte = currentByte[x / 8]; 
    
            // x % 8 resets our bit scanner to 0-7 for the current byte
            uint8_t bit_mask = 0x80 >> (x % 8);

            uint32_t fb_index = offs + (y * slots_per_row) + x;

            //  using colors fg for foreground color and bg for background
            // (we draw the foreground color when the bit in the bitmap is 1, and the bg color when is 0).
            if (font_byte & bit_mask) {
                fb_ptr[fb_index] = fg; 
                
            } else {
                fb_ptr[fb_index] = bg; 
            }


            
            
        }
    }
    
}

 
