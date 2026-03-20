# C++ Thread Pool Library 
A high-performance Thread Pool library built from scratch in C++ using low-level concurrency primitives.
# Benchmark Results
```
 Method     Time   Speedup 
 Sequential 4054ms  1x      
 Parallel   1008ms *4.02x faster* 
```
> 8 tasks across 4 worker threads
# Features
 Thread-safe task queue using mutex + condition variables
- Result retrieval via std::future and std::promise
- Real-time stats dashboard (active workers, queue depth, completion rate)
- Benchmarking system proving 4x speedup
- Graceful shutdown with RAII
# Concepts Used
- `std::thread` — worker thread management
- `std::mutex` — thread-safe queue access
- `std::condition_variable` — efficient thread sleeping/waking
- `std::future` + `std::promise` — async result retrieval
- `std::atomic` — lock-free counter updates
- Templates — generic task support for any return type
- RAII — automatic resource cleanup via destructor
# Project Structure
```
ThreadPool/
├── ThreadPool.h      ← class blueprint
├── ThreadPool.cpp    ← implementation
└── main.cpp          ← benchmark demo
```
# How to run
1. Clone the repository
2. Open `ThreadPool.sln` in Visual Studio
3. Build → `Ctrl + F5`

##  How It Works
```
main.cpp
    │
    ├── creates ThreadPool(4 workers)
    ├── enqueues 8 tasks
    │       │
    │       ▼
    │   taskQueue [task1...task8]
    │       │
    │       ▼
    │   4 workers grab tasks simultaneously
    │   results returned via future.get()
    │
    └── prints benchmark comparison
```
##  Key Design Decisions

- **workerLoop** fools threads into permanent waiting state
  — reusing them instead of expensive create/destroy cycles
- **condition_variable** eliminates busy-waiting
  — zero CPU usage when queue is empty
- **atomic counters** for stats
  — lighter than mutex for simple increments
