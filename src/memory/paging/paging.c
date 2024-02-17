#include "paging.h"
#include "memory/heap/kheap.h"
#include <stdbool.h>
#include "status.h"

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

bool paging_is_aligned(void* addr)
{
    return ((uint32_t)addr % PAGING_PAGE_SIZE) == 0;
}

// Convert vaddr to its index in the directory table and index in the paging table
int paging_get_indexes(void* v_address,uint32_t* directory_index_out,uint32_t* table_index_out)
{
    int res = 0;

    if(!paging_is_aligned(v_address))
    {
        res = -EINVARG;
        goto out;
    }

    // Get bits 31-22 -- Alternative: dt_index * 1024 * 4096 + offset = (address of the start of the block(devided by 4096) + offset)
    // -->  dt_index = (address+offset) / (1024 * 4096)
    *directory_index_out = (uint32_t)v_address >> 22;
    *table_index_out = ((uint32_t)v_address >> 12) & 0x000003FF;

out:
    return res;
}

int paging_set(uint32_t* directory, void* virt, uint32_t val)
{
   if(!paging_is_aligned(virt))
   {
      return -EINVARG;       
   }

   uint32_t directory_index = 0;
   uint32_t table_index = 0;
   int res = paging_get_indexes(virt,&directory_index,&table_index);

   if(res < 0)
   {
    return res;
   }

   // Get the directory table entry 
   // which containes the address for the page table
   uint32_t entry = directory[directory_index];
   // Extract the address from the entry and store it in the page table pointer
   uint32_t* table = (uint32_t*)(entry & PAGING_DIR_ENTRY_ADDR_MASK);
   // In the page table go to the page index and set the physical address
   table[table_index] = val;
   return 0;
}