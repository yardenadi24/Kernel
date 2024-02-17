#include "io/io.h"
#include "disk.h"
#include "memory/memory.h"
#include "config.h"
#include "status.h"

struct disk disk;

int disk_read_sector(int lba,int total, void* buffer)
{
    // Select master drive
    outsb(0x1F6, (lba>>24 | 0xE0)); //send lba 27-24 , use lba and master drive
    outsb(0x1F2, total); // send total sectors to read
    outsb(0x1F3, (unsigned char)(lba & 0xff)); // send lba 0-7
    outsb(0x1F4, (unsigned char)(lba >> 8)); // send lba 8-15
    outsb(0x1F5, (unsigned char)(lba >> 16)); // send lba 16-23 
    // total sent: 0-23 bits 
    outsb(0x1F7, 0x20); // send ccommand: read with retry

    unsigned short* ptr = (unsigned short*) buffer;
    for(int b = 0; b < total; b++)
    {
        // Wait for the buffer to be ready
        char c = insb(0x1F7);
        while(!(c & 0x08))
        {
            c = insb(0x1F7);
        }

        // Copy from hard disk to memory
        for(int i = 0; i<256; i++)
        {
            *ptr = insw(0x1F0);
            ptr++;
        }
    }
    return 0;
}


void disk_search_and_init()
{
    memset(&disk,0,sizeof(disk));
    disk.type = PEACHOS_DISK_TYPE_REAL;
    disk.sector_size = PEACHOS_SECTOR_SIZE;
}

struct disk* disk_get(int index)
{
    if (index != 0)
    {
        return 0;
    }

    return &disk;
}

int disk_read_block(struct disk* idisk, unsigned int lba, int total, void* buffer)
{
    if(idisk != &disk)
    {
        return -EIO;
    }

    return disk_read_sector(lba,total,buffer);
}