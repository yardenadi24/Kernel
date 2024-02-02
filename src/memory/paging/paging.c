#include "paging.h"
#include "memory/heap/kheap.h"


static uint32_t* current_directory = 0;
void paging_load_directory(uint32_t* directory);

struct paging_4gb_chuck* paging_new_4gb(uint8_t flags) 
{
    // Allocate the directory table
    uint32_t* directory = kzalloc(sizeof(uint32_t) * PAGING_TOTAL_ENTRIES_PER_TABLE);
    int offset = 0;
    // Init directory table
    for(int i = 0; i <PAGING_TOTAL_ENTRIES_PER_TABLE ; i++)
    {
        // for each entry we need to create a pageing table
        uint32_t* entry = kzalloc(sizeof(uint32_t) * PAGING_TOTAL_ENTRIES_PER_TABLE);
        
        // for each page table init its entries: [31-12 : page address][11-0 : flas]
        for(int j=0; j < PAGING_TOTAL_ENTRIES_PER_TABLE; j++)
        {
            entry[j] = (offset + (j * PAGING_PAGE_SIZE)) | flags;
        }

        offset += (PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE);
        directory[i] = (uint32_t)entry | flags | PAGEING_IS_WRITEABLE;
    }

    struct paging_4gb_chuck* chunck_4gb = kzalloc(sizeof(struct paging_4gb_chuck));
    chunck_4gb->directory_entry = directory;

    return chunck_4gb;
    
}

void paging_switch(uint32_t* directory)
{
    paging_load_directory(directory);
    current_directory = directory;
}

uint32_t* paging_4gb_chunck_get_directory(struct paging_4gb_chuck* chunck)
{
    return chunck->directory_entry;
}