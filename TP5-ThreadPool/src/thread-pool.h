/**
 * File: thread-pool.h
 * -------------------
 * This class defines the ThreadPool class, which accepts a collection
 * of thunks (which are zero-argument functions that don't return a value)
 * and schedules them in a FIFO manner to be executed by a constant number
 * of child threads that exist solely to invoke previously scheduled thunks.
 */

#ifndef _thread_pool_
#define _thread_pool_

#include <cstddef>
#include <functional>
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include "Semaphore.h"

class ThreadPool {
public:
    ThreadPool(size_t numThreads);
    void schedule(const std::function<void(void)>& thunk);
    void wait();
    ~ThreadPool();

private:
    struct Worker {
        std::thread thread;
        std::function<void(void)> task;
        Semaphore workerSem{0};
        bool available = true;
        bool taskReady = false;
    };

    void dispatcher();
    void worker(size_t id);

    std::vector<Worker> workers;

    std::queue<std::function<void(void)>> taskQueue;
    std::mutex taskQueueMutex;

    std::mutex workerMutex;

    bool shuttingDown = false;        
    size_t activeTasks = 0;
    std::mutex activeTasksMutex;
    std::condition_variable allDoneCv;

    std::thread dispatcherThread;
    Semaphore tasksInQueue{0};
    Semaphore workersAvailable{0};

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;
};

#endif // _thread_pool_
