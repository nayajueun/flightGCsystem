#include <stdio.h>
#include <stdlib.h>
#include "gc.h"
#include "flight.h"

int main(void) {

    /*
    char* str = scanlinedyn();
    printf("You entered >>>%s<<<", str);
    free(str);
    */

    setup_gc_roots();
    // wouldn't the size of flight vary depending on 
    flights = gc_malloc(sizeof(struct flight) * BLOCK_INCREMENT_SIZE);

    if (!flights) {
        printf("\ncalloc in initializing flight block failed.. exiting..\n");
        exit(1);
    }

    current_max_flight += BLOCK_INCREMENT_SIZE;
    printf("\nFlight block initalized to %i\n", current_max_flight);

    flight_schedule();

    if (ptrToBlock(flights)) {
        printf("True");
    }
    
    // printf("free `flights`\n");
    // gc_free(flights);

    printf("garbage collect to mark and sweep\n");
    gc_collect();

    printf("see if everything was cleaned up.\n");
    clean_up();
    printf("\nGoodbye!\n\n");
    return 0;

}