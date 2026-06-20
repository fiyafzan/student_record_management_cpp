# student_record_management_cpp

Student Record Management System (C++)
A console-based Student Record Management System built in C++ using a custom array-based data structure — no STL containers (no vector, map, etc.). Built as a personal/academic exercise in implementing core data structures and analyzing algorithmic complexity from first principles.

Features
- Add / Delete student records
- Search by Student ID (exact match) and by Name (case-insensitive partial match, returns all     matches)
- Display all records in a formatted table
- Sort by CGPA (ascending and descending) using a custom Selection Sort implementation
- Validation & error handling:
  > Student ID format (TP + 6 digits)
  > Contact Number format (XXX-XXXXXXX) and uniqueness
  > CGPA range (0.0–4.0)
  > Year of Study bounded by each programme's actual duration
  > Duplicate ID prevention
  > Non-numeric input handling (no crashes or infinite loops on bad input)
- Dynamic memory allocation (heap-allocated array) to safely support large datasets without stack overflow


Tech / Concepts Used
- C++ (structs, pointers, dynamic memory, file I/O, <chrono>)
- Custom array-based data structures (built without STL containers)
- Empirical Big-O complexity analysis — measured execution time across multiple dataset sizes     and matched against theoretical growth predictions
- CSV parsing with <sstream>


Performance Summary
Benchmarked across datasets of 500 / 2,000 / 8,000 / 30,000 records, with timing isolated to pure algorithmic execution (excluding user input time):

Operation            Big-O Time      Big-O Space
Add                  O(n)            O(1)
Delete               O(n)            O(1)
Linear Search        O(n)            O(1)
Selection Sort       O(n²)           O(1)
Display All          O(n)            O(1)

How to Run
1. Compile with any C++17-compatible compiler:
  > bash   g++ -std=c++17 -o student_system main.cpp
2. Make sure the dataset CSV files are in the same directory as the executable.
3. Run the executable and follow the on-screen menu.

Notes
This project focuses on implementing data structures and algorithms manually — sorting, searching, and dynamic array management — rather than relying on built-in containers or library functions, in order to understand their underlying mechanics and performance characteristics directly.
