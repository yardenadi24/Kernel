#ifndef PAGING_H
#define PAGING_H

#include <stdint.h>
#include <stddef.h>

#define PAGING_CACHE_DISABLED   0b00010000
#define PAGING_WRITE_THROUGH    0b00001000
#define PAGING_ACCESS_FROM_ALL  0b00000100
#define PAGEING_IS_WRITEABLE    0b00000010
#define PAGING_IS_PRESENT       0b00000001

#define PAGING_TOTAL_ENTRIES_PER_TABLE 1024
#define PAGING_PAGE_SIZE 4096
struct paging_4gb_chuck
{
    uint32_t* directory_entry;

};

void paging_load_directory();
void enable_paging();

struct paging_4gb_chuck* paging_new_4gb(uint8_t flags);
uint32_t* paging_4gb_chunck_get_directory(struct paging_4gb_chuck* chunck);
void paging_switch(uint32_t* directory);
#endif