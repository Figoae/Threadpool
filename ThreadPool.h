#pragma once
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <atomic>
#include <string>
#include <iostream>

class ThreadPool {
public:
    ThreadPool(int numThreads);
    ~ThreadPool();

    template<typename T>
    std::future<T> enqueue(std::function<T()> task) {
        auto promise = std::make_shared<std::promise<T>>();
        std::future<T> future = promise->get_future();
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            tasksInQueue++;
            taskQueue.push([promise, task, this]() {
                activeWorkers++;
                promise->set_value(task());
                activeWorkers--;
                tasksCompleted++;
                tasksInQueue--;
                });
        }
        condition.notify_one();
        return future;
    }

    // public stats methods
    void printStats(const std::string& label = "Stats");
    int getTasksCompleted();
    int getQueueSize();
    int getActiveWorkers();

private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> taskQueue;
    std::mutex queueMutex;
    std::condition_variable condition;
    bool stop;

    // atomic counters — thread safe
    std::atomic<int> tasksCompleted{ 0 };
    std::atomic<int> tasksInQueue{ 0 };
    std::atomic<int> activeWorkers{ 0 };
    int totalWorkers;

    void workerLoop();
};