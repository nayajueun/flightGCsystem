#ifndef FLIGHT_H
#define FLIGHT_H

#include "gc.h"
#define BLOCK_INCREMENT_SIZE 5

struct flight {
    char flight_number[7];
    char departure_time[5];
    char *destination;
    char **passengers;
    int num_passengers;
    int current_max_passenger;
};

struct flight *flights;  // Global declaration
int current_max_flight;

void add_flight();
void add_passenger(int flight_index);
void flight_schedule();
void flight_detail(int flight_index);
void clean_up();
void gc_mark_roots_impl();  // Declaration of the function
void setup_gc_roots();



#endif // FLIGHT_H
