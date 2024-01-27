#ifndef IO_H
#define IO_H

// Read one byte from the port
unsigned char insb(unsigned short port);
// Read one ward from the port
unsigned short insw(unsigned short port);

// Write char to the port
void outsb(unsigned short port,unsigned char val);
// Write word to the port
void outsw(unsigned short port,unsigned short val);


#endif