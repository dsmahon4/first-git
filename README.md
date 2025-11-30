# Memory Management Simulator

**A C++ Project by Dan Mahon**


## Project Purpose
This program is an educational tool designed to help students visualize how memory works in C++. It simulates the two main areas of memory:
* **The Stack:** Where local variables live (addresses start at `0x1000`).
* **The Heap:** Where dynamic memory lives (addresses start at `0x5000`).

The goal is to help users understand **pointers**, **manual memory management**, and how to avoid errors like **memory leaks** and **dangling pointers**.

## How to Run

### Dependencies
* You only need a standard C++ compiler
* No external libraries are required.

## Command List

The simulator works like a command line. Type these commands to interact with memory:

| Command | Example | Description |

| **int** | `int x = 10` | Creates a variable on the Stack. |
| **int*** | `int* p = &x` | Creates a pointer to a Stack variable. |
| **new** | `int* p = new int(5)` | Allocates new memory on the Heap. |
| **delete** | `delete p` | Frees Heap memory (prevents leaks). |
| **function** | `function myFunc` | Enters a new function scope. |
| **return** | `return` | Exits the function and removes local variables. |
| **show** | `show` | Displays the current memory map. |
| **check** | `check` | Scans for memory leaks or errors. |
| **clear** | `clear` | Resets the simulation. |
| **quit** | `quit` | Exits the program. |

##  Features & Error Checking
This program doesn't just run code; it checks for common C++ mistakes:
* **Leak Detection:** If you `new` without `delete`, the `check` command will warn you.
* **Double Free:** Prevents you from deleting the same pointer twice.
* **Safety:** Stops you from accessing memory that has already been freed.
