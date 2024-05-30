# Flight Booking Memory Management System

## Overview

This project is an implementation of a flight booking system in C, enhanced with a custom garbage collection system aimed at optimizing memory usage dynamically. The motivation stemmed from a need identified during a learning project on Coursera, which was extended to include more efficient memory management techniques.

## Usage

This section outlines the necessary commands to compile and run the application and its associated tests using `make`. Ensure you have `gcc` installed on your system to proceed with the following instructions.

### Compiling the Application

To compile the main application, use the provided Makefile. You can build the project by running the following command in the terminal:

```bash
make all
```
This will compile the main.c, gc.c, and flight.c files located in the src directory and link them to create an executable named main.

### Running the Application
Once compiled, run by executing:
```bash
./main
```


## System Design

### Memory Block Structure

```c
typedef struct MemBlock {
    struct MemBlock* next;
    size_t size;
    bool marked;
    char data[];
} MemBlock;

```
The `MemBlock` structure manages each allocated block of memory, incorporating metadata about the size and whether the block is marked during garbage collection. The `data[]` is a flexible array member holding the actual flight data, dynamically sized at runtime.

### Garbage Collection Mechanics
The system implements a basic mark-and-sweep garbage collector:
- **Mark Phase:** Traverses and marks all reachable memory blocks starting from root references.
- **Sweep Phase:** Frees all memory blocks that are not marked, effectively reclaiming unused memory.

The system easily integrates into manually managed systems, enhacing them without the complexity of more advanced garbage collection techniques. But the system implements "stop-the-world" garbage collection which, while simple can cause performance bottlenecks in larger systems.

#### Further Improvements
Currently, the system does not support marking objects from their references automatically. The planned improvements aim to enhance the system’s ability to mark all objects reachable from the roots automatically, thereby streamlining the marking process for complex nested structures like our flight booking data.

### Optimization
- **Generational Garbage Collection:** Introducing generational collection could optimize the management of short-lived objects, reducing the overhead of marking and sweeping frequently.
- **Parallel Mark-and-Sweep:** Implementing a parallel or concurrent garbage collector could minimize the impact of garbage collection pauses on the application performance.

## Conclusion
The flight booking memory management system serves as a robust platform for understanding and implementing custom memory management in C. As the system evolves, incorporating more advanced garbage collection strategies will be crucial to handle larger datasets and more complex interactions efficiently. This project highlights the balance between implementing low-level memory management techniques and the need for sophisticated, automated solutions found in higher-level languages.


## Acknowledgments

The initial structure of the flight management system was inspired by the [Managing Memory with C Programming course](https://www.coursera.org/projects/managing-memory-c-programming) on Coursera. Modifications and additional features like the custom garbage collection system were independently developed for this project.