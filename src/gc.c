#include "gc.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


MemBlock* heap_head = NULL;

// find the block from a given pointer
MemBlock* ptrToBlock(void* ptr) {
    char* checkPtr = (char*) ptr; 
    // Cast void* to char* -> treating the pointer in a way that allows byte-level manipulation (casting is essential!)
    // checkPtr = address pointed to by ptr
    MemBlock* current = heap_head;
    while (current != NULL) {
        char* start = &current->data[0];  // Starting address of the managed memory area within a MemBlock
        //  immediately following the metadata of MemBlock.
        char* end = start + current->size; // End of the data block
        // Memory addresses are consecutive, meaning each byte has a unique address, 
        // and each subsequent unit's address is incremented by one from the previous. 
        // This consecutive nature allows for the use of pointer arithmetic.

        if (start <= checkPtr && checkPtr < end) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}


// Utility to add block to the managed heap
void addBlockToHeap(MemBlock* block) {
    block->next = heap_head;
    heap_head = block;
}

// Allocate memory and add block to heap
void* gc_malloc(size_t size) {
    MemBlock* block = malloc(sizeof(MemBlock) + size);
    if (!block) {
        fprintf(stderr, "Out of memory\n");
        exit(EXIT_FAILURE);
    }
    block->size = size;
    block->marked = false;
    addBlockToHeap(block);
    return block->data;
}

void* gc_calloc(size_t num_elements, size_t element_size) {
    size_t total_size = num_elements * element_size;
    void* ptr = gc_malloc(total_size); // Use gc_malloc to allocate memory
    if (ptr) {
        memset(ptr, 0, total_size); // Initialize allocated memory to zero
    }
    return ptr;
}


void* gc_realloc(void* ptr, size_t new_size) {
    if (ptr == NULL) return gc_malloc(new_size);

    MemBlock* block = ptrToBlock(ptr);
    if (!block) return NULL; // ptr not managed

    void* new_ptr = gc_malloc(new_size);
    if (new_ptr) {
        memcpy(new_ptr, ptr, block->size < new_size ? block->size : new_size);
        // Optionally, mark the old block for collection if no longer needed
        // This requires a way to untrack the old block, e.g., removing it from the linked list
    }
    return new_ptr;
}

void gc_free(void* ptr) {
    MemBlock* block = ptrToBlock(ptr);
    if (block) {
        // Remove from linked list logic here
        free(block); // free the entire block, not just the data
    } else {
        printf("Already freed by sweep or has not been allocated in the first place\n");
    }
}


void mark(void* ptr) {
    MemBlock* block = ptrToBlock(ptr);
    if (block == NULL || block->marked) {
        return;
    }
    block->marked = true;

    // Assuming each pointer (such as object references in a high-level language) 
    // is stored at the start of a block, and we need to mark recursively
    // This setup is typical in garbage collection systems where memory blocks can contain references to other memory blocks.
    void** p = (void**)&block->data[0];

    for (size_t i = 0; i < block->size / sizeof(void*); ++i) {
        mark(p[i]);
        // p[i] is a pointer, potentially a refernce to other memory blocks that also need to be managed and marked by the garbage collector.
    }
}

void sweep() {
    MemBlock** curr = &heap_head;
    while (*curr != NULL) {
        if ((*curr)->marked) {
            (*curr)->marked = false; // Unmark for the next GC cycle
            curr = &(*curr)->next;
        } else {
            MemBlock* unreached = *curr;
            *curr = unreached->next;
            gc_free(unreached);
        }
    }
}

void (*gc_mark_roots)(void) = NULL;

void gc_collect() {
    // Unmark all blocks
    for (MemBlock* block = heap_head; block != NULL; block = block->next) {
        block->marked = false;
    }
    // Mark phase
    if (gc_mark_roots != NULL) {
        gc_mark_roots();  // Call user-defined function to mark roots
    } else {
        fprintf(stderr, "Root marking function not set, and hence, you must've marked manually.\n");

    }

    // Sweep phase
    sweep();
}

// data[0] is a versatile component of the memory management system. 
// It serves as the critical link between the abstract management of memory blocks (MemBlock) and the concrete handling of the user data stored within these blocks. 
// In both boundary checking and garbage collection marking, data[0] provides the necessary interface for treating a contiguous block of memory either as raw data or 
// as structured data comprising pointers that need recursive management. 
// This dual use is crucial for implementing effective and efficient garbage collection strategies in systems programmed in C.

// struct flight example, there are indeed pointers (destination and passengers) that reference other memory blocks. However, these pointers are not located at the beginning of the memory block managed by MemBlock (i.e., not at data[0] specifically). They are embedded within the data structure that follows the initial bytes of flight_number and departure_time.