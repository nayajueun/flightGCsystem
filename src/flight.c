#include "flight.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



// struct flight flights[5];
struct flight *flights = NULL;  // Global definition
int current_max_flight = 0;
int num_flight = 0;

void gc_mark_roots_impl() {
    for (int i = 0; i < num_flight; i++) {
        mark(flights[i].destination);
        for (int j = 0; j < flights[i].num_passengers; j++) {
            mark(flights[i].passengers[j]);
        }
    }
    mark(flights);
}

void setup_gc_roots() {
    gc_mark_roots = gc_mark_roots_impl;
}


/* a utility function to read a line including whitespaces */
/* but excludes newline characters */

void scanline(char* str, int max_size) {

    int i = 0;
    int ch;

    fflush(stdin);

    do {
        ch = getc(stdin);

        if (i < max_size && ch != '\n' && ch != EOF) {
            str[i] = ch;
            i++;
        }
    
    } while (ch != '\n' && ch != EOF);

    str[i] = 0;

}

char* scanlinedyn() {

    #define INITIAL_BUFFER_SIZE 20
    #define BUFFER_INCREMENT 10

    int current_buffer_size = INITIAL_BUFFER_SIZE;
    char* str = gc_malloc(INITIAL_BUFFER_SIZE);  
    if (!str) {
        printf("\ngc_malloc failed.. exiting\n");
        exit(1);
    }

    int i = 0, ch;
    fflush(stdin);

    while ((ch = getc(stdin)) != '\n' && ch != EOF) {
        if (i == current_buffer_size) {
            str = gc_realloc(str, current_buffer_size + BUFFER_INCREMENT);  // use gc_realloc
            if (!str) {
                printf("\ngc_realloc failed.. exiting\n");
                exit(1);
            }
            current_buffer_size += BUFFER_INCREMENT;
        }
        str[i++] = ch;
    }
    str[i] = '\0';
    return str;
}


/* submenu to add a passenger-------------------------- */

void add_passenger(int flight_index) {
    
    // char new_passenger_name[41];
    int new_passenger_index;

    /* do a range check to be safe */

    if (flight_index < 0 || flight_index >= num_flight) {
        return;
    }

    puts("\n***** SUBMENU: ADD PASSENGER *****"); 

    printf("ENTER PASSENGER'S NAME: ");
    // scanline(new_passenger_name, 40);

    new_passenger_index = flights[flight_index].num_passengers;

    // do we need to allocate a new block of memory for more passengers?
    if (new_passenger_index == flights[flight_index].current_max_passenger) {
        flights[flight_index].passengers = gc_realloc(flights[flight_index].passengers,
        sizeof(char *) * (flights[flight_index].current_max_passenger + BLOCK_INCREMENT_SIZE));

        if (flights[flight_index].passengers) {
            flights[flight_index].current_max_passenger += BLOCK_INCREMENT_SIZE;
            printf("\nPassenger block expanded. New size = %i\n", flights[flight_index].current_max_passenger);
        } else {
            printf("\nrelloc in add_passenger failed.. exiting\n");
            exit(1);
        }

    }

    // strncpy(flights[flight_index].passengers[new_passenger_index], new_passenger_name, 40);
    flights[flight_index].passengers[new_passenger_index] = scanlinedyn();
    flights[flight_index].num_passengers ++;
    fflush(stdin);

    return;

}

/* submenu to add a flight ---------------------------- */

void add_flight() {
    char flight_number[7]; // Temporary storage for input
    char departure_time[5]; // Temporary storage for input
    char *destination; // Will point to dynamically allocated memory

    puts("\n********** SUBMENU: ADD FLIGHT **********");

    printf("ENTER FLIGHT NUMBER: (XXnnnn): ");
    scanf("%s", flight_number); // Read into temporary buffer

    printf("ENTER DEPARTURE TIME (hhmm): ");
    scanf("%s", departure_time); // Read into temporary buffer

    printf("ENTER DESTINATION: ");
    destination = scanlinedyn(); // Dynamically allocate and read destination

    // Allocate memory and copy the flight number and departure time into the struct
    // flights[num_flight].flight_number = malloc(strlen(flight_number) + 1);
    strcpy(flights[num_flight].flight_number, flight_number);

    strncpy(flights[num_flight].departure_time, departure_time, 4);
    flights[num_flight].departure_time[4] = '\0'; // Ensure null termination

    // Assign the dynamically allocated destination directly
    flights[num_flight].destination = destination;

    flights[num_flight].num_passengers = 0;
    flights[num_flight].current_max_passenger = 0;
    flights[num_flight].passengers = NULL; // Initialize pointer

    num_flight++;

    printf("%s to %s added!\n", flight_number, destination);
}


/* submenu to show details of a flight------------------ */

void flight_detail(int flight_index) {

    char input[2];
    int i;

    /* do a range check to be safe */

    if (flight_index < 0 || flight_index >= num_flight) {
        return;
    }

    /* loop until X is entered */

    while(strcmp(input, "X") != 0) {

        printf("\n********** SUBMENU: FLIGHT %s **********\n", flights[flight_index].flight_number);
        printf("DEPARTURE TIME: %s\n", flights[flight_index].departure_time);
        printf("DESTINATION: %s\n\n", flights[flight_index].destination);

        puts("***** PASSENGERS *****");

        /* show all passengers or a message */

        if (flights[flight_index].num_passengers == 0) {
            printf("\n-- No passengers yet, enter A to add --\n\n");
        }
        else {
            for (i = 0; i < flights[flight_index].num_passengers; i++) {
                printf("%i. %s\n",
                i + 1,
                flights[flight_index].passengers[i]);
            }
        }

        puts("\nA = ADD PASSENGER");
        puts("X = GO BACK");
        printf("ENTER OPTION: ");

        scanf("%s", input);
        
        if (strcmp(input, "A") == 0) {

            /* is there room for another passenger? */

            // if (flights[flight_index].num_passengers >= 5) {
            //     printf("*** THIS FLIGHT IS FULL ***");
            // }
            // else {
            add_passenger(flight_index);
            // }

            continue;
        }

        if (strcmp(input, "X") != 0) {
            printf("\n** %s IS NOT A INVALID OPTION **\n", input);
        }
    }

}

/* main menu to show the flight schedule of the day ---- */

void flight_schedule(void) {
    #define BLOCK_INCREMENT_SIZE 5

    char input[2];
    int i;
    char item_number;

    /* loop until X is entered */

    while(strcmp(input, "X") != 0) {
        puts("\n********** TODAY'S FLIGHT SCHEDULE **********");

        /* show all existing flights or a message */

        if (num_flight == 0) {
            printf("\n-- No flights scheduled yet, enter A to add --\n\n");
        }
        else {
            for (i = 0; i < num_flight; i++) {
                printf("%i. %s %s %s\n",
                i + 1,
                flights[i].flight_number,
                flights[i].departure_time,
                flights[i].destination);
            }

            puts("\nITEM NUMBER = FLIGHT DETAILS");
        }

        puts("A = ADD FLIGHT");
        puts("X = EXIT");
        printf("ENTER OPTION: ");
        scanf("%s", input);

        /* See if "A" is entered */

        if (strcmp(input, "A") == 0) {

            /* is there still room for more flights? */
            if (num_flight == current_max_flight) {
                flights = gc_realloc(flights, sizeof(struct flight) * (current_max_flight + BLOCK_INCREMENT_SIZE));

                if (flights) {
                    current_max_flight += BLOCK_INCREMENT_SIZE;
                    printf("\nFlight block expanded. New size = %i\n", current_max_flight);
                } else {
                    printf("\nrelloc in adding flight schedule failed.. exiting\n");
                    exit(1);
                }
            }

            add_flight();
            continue;
        }

        /* See if a valid item number is entered */
        item_number = atoi(input);

        if (item_number > 0 && item_number <= num_flight) {
            flight_detail(item_number - 1);
            continue;
        }

        if (strcmp(input, "X") != 0) {
            printf("\n** %s IS NOT A INVALID OPTION **\n", input);
        }

    }

    return;

}

void clean_up() {
    int i, j;

    for (i = 0; i < num_flight; i++) {
        for (j = 0; j < flights[i].num_passengers; j++) {
            gc_free(flights[i].passengers[j]);
            printf("flights[%i].passengers[%i] released\n", i, j);
        }
        gc_free(flights[i].destination);
        printf("flight[%i].destination released.\n", i);
    }
    if (ptrToBlock(flights)) {
        printf("True");
    } else {
        printf("False");
    }
    
    gc_free(flights);
    printf("All flights released\n");
}



