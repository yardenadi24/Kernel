#ifndef DISK_H
#define DISK_H

typedef unsigned int PEACHOS_DISK_TYPE;

#define PEACHOS_DISK_TYPE_REAL 1;

struct disk
{
    PEACHOS_DISK_TYPE type;
    int sector_size;
};

int disk_read_block(struct disk* idisk, unsigned int lba, int total, void* buffer);
void disk_search_and_init();
struct disk* disk_get(int index);

#endif