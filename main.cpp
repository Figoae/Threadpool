#include <iostream>
#include <vector>
#include <chrono>
#include <functional>
#include "ThreadPool.h"

// helper function to measure time
long long measureTime(std::function<void()> benchmark) {
    auto start = std::chrono::high_resolution_clock::now();
    benchmark();
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast
        <std::chrono::milliseconds>(end - start).count();
}

// the actual task — same for both benchmarks
std::function<int()> makeTask(int i) {
    return [i]() {
        std::this_thread::sleep_for(
            std::chrono::milliseconds(500)
        );
        return i * 10;
        };
}

int main() {
    const int NUM_TASKS = 8;
    const int NUM_WORKERS = 4;

    std::cout << "=== ThreadPool Benchmark ===\n\n";

    
    // BENCHMARK 1: Sequential
    std::cout << "Running sequential benchmark...\n";

    long long seqTime = measureTime([&]() {
        for (int i = 1; i <= NUM_TASKS; i++) {
            auto task = makeTask(i);
            task();   
        }
        });

    std::cout << "Sequential done!\n\n";

    // BENCHMARK 2: Parallel (ThreadPool)
    
    std::cout << "Running parallel benchmark...\n";

    long long parTime = measureTime([&]() {
        ThreadPool pool(NUM_WORKERS);
        std::vector<std::future<int>> futures;

        for (int i = 1; i <= NUM_TASKS; i++) {
            futures.push_back(
                pool.enqueue<int>(
                    std::function<int()>(makeTask(i))
                )
            );
        }

        // wait for all tasks
        for (auto& f : futures) f.get();
        });
    //RESULTS
    std::cout << "Parallel done!\n\n";

    double speedup = (double)seqTime / (double)parTime;
    double efficiency = (speedup / NUM_WORKERS) * 100;

    std::cout << "========= Benchmark Results =========\n";
    std::cout << "Tasks          : " << NUM_TASKS << "\n";
    std::cout << "Workers        : " << NUM_WORKERS << "\n";
    std::cout << "Sequential     : " << seqTime << "ms\n";
    std::cout << "Parallel       : " << parTime << "ms\n";
    std::cout << "Speedup        : " << speedup << "x faster\n";
    std::cout << "Efficiency     : " << efficiency << "%\n";
    std::cout << "=====================================\n";

    return 0;
}
