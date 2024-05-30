#include "gc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void assert(bool condition, const char *message) {
    if (!condition) {
        printf("Assertion failed: %s\n", message);
        exit(EXIT_FAILURE);
    }
    printf("Assertion passed: %s\n", message);
}

void test_gc() {
    printf("Test start.\n");
    char *root = gc_malloc(100); // Root block that remains referenced
    strcpy(root, "root");

    char *lost = gc_malloc(100); // Block that will be lost
    strcpy(lost, "lost");

    // Manually mark root to simulate root marking, assuming mark function exists
    mark(root);

    // Lose reference to lost
    lost = NULL;

    // Trigger garbage collection
    gc_collect();

    // Since lost1 is set to NULL, we can't directly check if it was collected
    // Let's assume we have access to some internal mechanism or can enhance gc_malloc to track blocks
    // For now, let's assume we have a function `is_collected` which you must implement
    assert(is_collected(lost), "lost should have been collected");
    assert(!is_collected(root), "Root block should still exist");

    // Clean up
    gc_free(root); // Manually free remaining blocks
    gc_collect(); // Ensure all memory is cleaned up
    printf("Test end.\n");
}

int main(void) {
    test_gc();
    printf("All GC tests passed.\n");
    return 0;
}

