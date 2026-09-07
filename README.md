# Parallel Sudoku Validation using Multithreading

A multithreaded Sudoku validation project developed as part of the Operating Systems-II course at IIT Hyderabad. The project explores parallel task scheduling, synchronization, mutual exclusion, and performance analysis by progressively implementing static and dynamic workload distribution strategies.

## Overview

The project consists of two related implementations:

1. **Static Parallel Sudoku Validation** – validates Sudoku rows, columns, and sub-grids using POSIX threads with Chunk and Mixed workload allocation.
2. **Dynamic Parallel Sudoku Validation** – extends the first implementation by dynamically assigning validation tasks to threads using a shared counter and different mutual exclusion mechanisms.

The objective is to study how different scheduling and synchronization techniques affect the performance of parallel programs.

---

## Project Structure

### Assignment 1 — Static Sudoku Validation

The first implementation validates an `N × N` Sudoku using multiple threads.

A Sudoku is considered valid when:

- Every row contains unique values from `1` to `N`.
- Every column contains unique values from `1` to `N`.
- Every `√N × √N` sub-grid contains unique values from `1` to `N`.

The validation work is divided among three groups of threads:

- Row validation
- Column validation
- Sub-grid validation

Two static workload allocation strategies were implemented:

### Chunk Scheduling

Consecutive rows, columns, or sub-grids are assigned to each thread.

For example, if there are `R` rows and `K` threads, each thread receives a consecutive portion of the rows.

### Mixed Scheduling

The workload is distributed cyclically among threads.

For example, with `K` threads:

- Thread 1 checks rows `1, K+1, 2K+1, ...`
- Thread 2 checks rows `2, K+2, 2K+2, ...`
- and so on.

The performance of both strategies was compared against sequential execution.

---

## Assignment 2 — Dynamic Sudoku Validation

The second implementation extends the static version by introducing **dynamic task scheduling**.

Instead of assigning fixed tasks to threads beforehand, threads obtain their next task dynamically using a shared counter `C`.

The process works as follows:

1. A thread enters the critical section.
2. It increments the shared counter by `taskInc`.
3. The increment determines the next group of rows, columns, or sub-grids to validate.
4. The thread leaves the critical section.
5. It performs the assigned validation.
6. After completing the task, the thread obtains another task until all validation work is completed.

This approach allows the workload to be distributed dynamically among the available threads.

---

## Synchronization and Mutual Exclusion

Since multiple threads access and modify the shared counter, synchronization is required to prevent race conditions.

Three mutual exclusion techniques were implemented and compared:

- **Test-and-Set (TAS)**
- **Compare-and-Swap (CAS)**
- **Bounded CAS**

The implementations use atomic operations to safely coordinate access to the shared counter.

The project also includes early termination, where validation can stop when a thread identifies that the Sudoku is invalid.

---

## Performance Analysis

The project evaluates the performance of different scheduling and synchronization strategies through multiple experiments.

### Assignment 1 Experiments

The static implementation compares:

- Sequential execution
- Chunk scheduling
- Mixed scheduling

Performance is evaluated by varying Sudoku size while keeping the number of threads fixed, and by varying the number of threads while keeping the Sudoku size fixed.

### Assignment 2 Experiments

The dynamic implementation compares:

- TAS
- CAS
- Bounded CAS
- Sequential execution

The experiments study:

1. **Execution Time vs Sudoku Size**
   - Number of threads fixed at 8
   - `taskInc` fixed at 20

2. **Execution Time vs Task Increment**
   - Sudoku size fixed at `90 × 90`
   - Number of threads fixed at 8

3. **Execution Time vs Number of Threads**
   - Sudoku size fixed at `90 × 90`
   - `taskInc` fixed at 20

Each experiment is repeated multiple times and the results are analyzed using plots and tables.

The analysis includes:

- Total execution time
- Average critical-section entry time
- Average critical-section exit time
- Worst-case critical-section entry time
- Worst-case critical-section exit time
- Effect of thread count and task size on performance
- Comparison of different mutual exclusion mechanisms

---