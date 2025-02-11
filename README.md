This project implements a virtual machine capable of executing the custom RISK-XVII instruction set, a reduced version of the RV32I architecture. The virtual machine simulates a 32-bit processor with memory-mapped components, including instruction memory, data memory, virtual routines, and heap banks. It supports arithmetic operations, memory access, program flow control, and dynamic memory allocation. The implementation includes error handling, I/O operations, and a register dump mechanism to aid debugging.

This repository contains:

Virtual Machine Implementation: Written in C, following structured and efficient memory management.
RISK-XVII Instruction Set Emulator: Supports 33 distinct instructions, including arithmetic, logic, and branching operations.
Memory Management System: Implements a hardware-managed heap allocation mechanism.
Virtual Routines for I/O: Handles console input/output operations.
Test Cases and Execution Scripts: Ensure correctness through automated testing.
