#include "ThreadPool.h"

ThreadPool::ThreadPool(int numThreads)
    : stop(false), totalWorkers(numThreads) {
    std::cout << "Starting thread pool with "
        << numThreads << " workers...\n\n";
    for (int i = 0; i < numThreads; i++)
        workers.push_back(
            std::thread(&ThreadPool::workerLoop, this)
        );
}

ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        stop = true;
    }
    condition.notify_all();
    for (std::thread& worker : workers)
        worker.join();
    std::cout << "\nThread pool shut down cleanly.\n";
}

void ThreadPool::printStats(const std::string& label) {
    std::cout << "\n========== " << label << " ==========\n";
    std::cout << "Total workers   : " << totalWorkers << "\n";
    std::cout << "Active workers  : " << activeWorkers << "\n";
    std::cout << "Tasks in queue  : " << tasksInQueue << "\n";
    std::cout << "Tasks completed : " << tasksCompleted << "\n";
    std::cout << "================================\n\n";
}

int ThreadPool::getTasksCompleted() { return tasksCompleted; }
int ThreadPool::getQueueSize() { return tasksInQueue; }
int ThreadPool::getActiveWorkers() { return activeWorkers; }

void ThreadPool::workerLoop() {
    while (true) {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            condition.wait(lock, [this]() {
                return !taskQueue.empty() || stop;
                });
            if (stop && taskQueue.empty()) return;
            task = taskQueue.front();
            taskQueue.pop();
        }
        task();
    }
}