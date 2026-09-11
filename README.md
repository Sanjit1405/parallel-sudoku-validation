# Multithreading, Scheduling & Synchronization

A collection of multithreading and synchronization implementations in C/C++, covering parallel task scheduling, atomic synchronization primitives, and the bounded-buffer Producer–Consumer problem.

## Overview

This project explores fundamental concepts in concurrent programming through three related assignments:

1. **Parallel Sudoku Validation**
2. **Dynamic Task Allocation & Atomic Synchronization**
3. **Producer–Consumer using Semaphores and Locks**

The implementations use multiple threads to perform parallel computation and investigate how scheduling strategies and synchronization mechanisms affect performance and scalability.

---

## 1. Parallel Sudoku Validation

Implemented a multithreaded Sudoku validator using POSIX threads.

### Approach

The Sudoku grid is divided into independent validation tasks:

- Row validation
- Column validation
- Subgrid validation

Each task is assigned to a worker thread.

### Scheduling Strategies

Two static scheduling strategies were implemented:

- **Chunk Scheduling** – consecutive validation tasks are assigned to threads.
- **Mixed Scheduling** – tasks are distributed cyclically among threads.

The implementation was evaluated by varying:

- Sudoku grid size
- Number of threads

This was used to study workload distribution and parallel performance.

---

## 2. Dynamic Task Allocation & Atomic Synchronization

Extended the Sudoku validation system with dynamic task scheduling.

Instead of assigning tasks statically, worker threads dynamically obtain the next available task using a shared task counter.

### Dynamic Scheduling

A shared counter `C` is used to track the next task.

Each thread:

1. Obtains the current task index.
2. Atomically increments the shared counter.
3. Processes the assigned task.
4. Repeats until all tasks are completed.

### Synchronization Mechanisms

Implemented and compared multiple atomic synchronization techniques:

- **Test-and-Set (TAS)**
- **Compare-and-Swap (CAS)**
- **Bounded Compare-and-Swap (Bounded CAS)**

These mechanisms provide mutual exclusion when multiple threads access the shared task counter.

### Performance Analysis

Performance was evaluated by varying:

- Sudoku size
- Number of threads
- Task increment size
- Synchronization mechanism

The experiments were used to analyze synchronization overhead, task distribution, and scalability.

---

## 3. Producer–Consumer using Semaphores and Locks

Implemented the classical **bounded-buffer Producer–Consumer problem** using multiple producer and consumer threads.

### Components

- Shared bounded buffer
- Producer threads
- Consumer threads
- Synchronization mechanisms

Two synchronization approaches were implemented:

- **Semaphores**
- **Locks**

Producers insert items into the bounded buffer, while consumers remove them. Synchronization ensures safe access to the shared buffer and prevents invalid operations such as inserting into a full buffer or removing from an empty buffer.

### Performance Analysis

The implementations were compared by varying:

- Producer delay
- Consumer delay
- Number of producer threads
- Number of consumer threads

Execution times were measured to study how synchronization mechanisms and workload ratios influence system performance.

---

## Technologies Used

- **C / C++**
- **POSIX Threads (pthreads)**
- **Semaphores**
- **Locks / Mutual Exclusion**
- **Atomic Operations**
- **Test-and-Set (TAS)**
- **Compare-and-Swap (CAS)**
- **Bounded CAS**
- **Multithreading**

---


