#ifndef SERIAL_H
#define SERIAL_H

#include <stdint.h>

int init_serial();
char read_serial();
void write_serial(char a);
void print_serial(char[]);

#endif // SERIAL_H
