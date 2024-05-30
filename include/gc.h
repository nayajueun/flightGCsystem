#ifndef GC_H
#define GC_H

#include <stddef.h>
#include <stdbool.h>

typedef struct MemBlock {
    struct MemBlock* next;
    size_t size;
    bool marked;
    char data[]; // flexible storage area for arbitrary data
} MemBlock;

extern void (*gc_mark_roots)(void);

void* gc_malloc(size_t size);
void* gc_calloc(size_t num_elements, size_t element_size);
void* gc_realloc(void* ptr, size_t new_size);
void gc_free(void* ptr);
void gc_collect();
void mark(void* ptr);
void sweep();
MemBlock* ptrToBlock(void* ptr);

#endif // GC_H
