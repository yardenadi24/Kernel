#include "heap.h"
#include "kernel.h"
#include "status.h"
#include "memory/memory.h"

static int heap_validate_table(void* ptr, void* end, struct heap_table* table)
{
    int res = 0;

    size_t table_size = (size_t)(end-ptr);
    size_t total_blocks = table_size / PEACHOS_HEAP_BLOCK_SIZE;
    if (table->total != total_blocks)
    {
        res = -EINVARG;
        print("Failed heap validation\n");
        goto out;
    }
out:
    return res;
}

static int heap_validate_alignment(void* ptr)
{
    return ((unsigned int)ptr % PEACHOS_HEAP_BLOCK_SIZE) == 0;
}

int heap_create(struct heap* heap, void* ptr, void* end, struct heap_table* table)
{
    int res = 0;

    // Validate heap start to end is actually in jumps of full block sizes
    if (!heap_validate_alignment(ptr) || !heap_validate_alignment(end))
    {
        print("Failed heap_validate_alignment of start and end when creating heapc");
        res = -EINVARG;
        goto out;
    }

    // Init all heap to 0 bytes
    memset(heap,0,sizeof(struct heap));
    heap->saddr = ptr;
    heap->table = table;

    res = heap_validate_table(ptr,end,table);
    if(res<0)
    {
        print("Failed creating heap\n");
        res = -EINVARG;
        goto out;
    }

    size_t table_size = sizeof(HEAP_BLOCK_TABLE_ENTRY) * table->total;
    memset(table->entries,HEAP_BLOCK_TABLE_ENTRY_FREE,table_size);

out:
    return res;
}
static int heap_get_entry_type(HEAP_BLOCK_TABLE_ENTRY entry)
{
    return entry & 0x0f;
}
int heap_get_start_block(struct heap* heap, uint32_t total_blocks)
{
    struct heap_table* table = heap->table;
    int bc = 0;
    int bs = -1;
    for(size_t i = 0 ; i < table->total;i++)
    {
        if(heap_get_entry_type(table->entries[i])!=HEAP_BLOCK_TABLE_ENTRY_FREE)
        {
            // reset state
            bc = 0;
            bs = -1;
            continue;
        }

        if(bs == -1)
        {
            bs = i;
        }

        bc++;
        
        if(bc == total_blocks)
        {
            break;
        }

    }
    if (bs == -1)
    {
        print("Failed gettting start block\n");
        return -ENOMEM;
    }
    return bs;
}

void* heap_block_to_address(struct heap* heap, int block)
{
   return heap->saddr + (block*PEACHOS_HEAP_BLOCK_SIZE);
}

void heap_mark_blocks_taken(struct heap* heap, int start_block,int total_blocks)
{
    int end_block = (start_block + total_blocks) -1;
    
    //first block
    HEAP_BLOCK_TABLE_ENTRY entry = HEAP_BLOCK_TABLE_ENTRY_TAKEN | HEAP_BLOCK_IS_FIRST;
    if(total_blocks > 1)
    {
        entry = entry | HEAP_BLOCK_HAS_NEXT;
    }
    
    for(int i = start_block ; i<= end_block;i++)
    {
        heap->table->entries[i] = entry;

        // Prepare for next iteration
        entry = HEAP_BLOCK_TABLE_ENTRY_TAKEN;
        if(i != (end_block-1))
        {   
            entry |= HEAP_BLOCK_HAS_NEXT;
        }

    }
}

static uint32_t heap_align_value_to_upper(uint32_t val)
{
    uint32_t reminder = val % PEACHOS_HEAP_BLOCK_SIZE;
    if (reminder > 0 )
    {
        val = ((val - reminder) + PEACHOS_HEAP_BLOCK_SIZE);
    }
    return val;
}
void* heap_mallock_blocks(struct heap* heap, uint32_t total_blocks)
{
    void* address = 0;

    int start_block = heap_get_start_block(heap,total_blocks);
    if(start_block<0)
    {
        // Not enough memory
        goto out;
    }

    // Convert the block index to address
    address = heap_block_to_address(heap,start_block);

    // Mark blocks taken
    heap_mark_blocks_taken(heap,start_block,total_blocks);

out:    
    return address;
}   
void* heap_malloc(struct heap* heap, size_t size){
    size_t alinged_size = heap_align_value_to_upper(size);
    uint32_t num_of_blocks = alinged_size / PEACHOS_HEAP_BLOCK_SIZE;
    return heap_mallock_blocks(heap,num_of_blocks);
}

void heap_mark_blocks_free(struct heap* heap,int block)
{
    for(int i = block; i < (int)(heap->table->total); i ++)
    {
        HEAP_BLOCK_TABLE_ENTRY entry = heap->table->entries[i];
        heap->table->entries[i] = HEAP_BLOCK_TABLE_ENTRY_FREE;      
        if(!(entry & HEAP_BLOCK_HAS_NEXT))
        {
            break;
        }
    }
}

int heap_address_to_block(struct heap* heap,void* ptr)
{
    return ((int)(ptr - heap->saddr)) / PEACHOS_HEAP_BLOCK_SIZE;
}

void heap_free(struct heap* heap, void* ptr){
    heap_mark_blocks_free(heap,heap_address_to_block(heap, ptr));
}
