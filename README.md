# Memory Management Simulator

**Author:** Dan Mahon


## Description
This project is a CLI-based simulation tool designed to help users visualize the difference between Stack and Heap memory in C++. It allows users to declare variables, allocate memory dynamically, and observe how pointers function in a safe environment. The program also includes error detection to identify memory leaks and dangling pointers.

## Features
* Visualizes Stack memory (local variables) and Heap memory (dynamic allocations).
* Simulates variable scope with function calls and returns.
* Detects common memory errors like memory leaks and double deletion.
* Validates user input to ensure proper C++ syntax.

## Dependencies
* A standard C++ compiler
* No external libraries are required.

## Usage
The program accepts commands that mimic C++ syntax. Below are the available commands:

* `int <name> = <value>`
  Declare integer variable

* `int* <pointer> = &<int name>`
  Declare pointer variable

* `int* <pointer> = new int(<value>)`
  Allocate memory for variable on heap

* `<int> = <value>`
  Assign new value

* `*<pointer> = <value / int name>`
  Dereference and assign

* `<pointer> = &<int name>`
  Assign new integer variable address

* `delete <pointer>`
  Free heap memory

* `<pointer> = nullptr`
  Set pointer to null

* `function <name>`
  Enter new function

* `return`
  Exit current function

* `show`
  Display memory state

* `check`
  Check for memory errors

* `clear`
  Reset simulator

* `help`
  Display list of commands

* `quit`
  Exit program
