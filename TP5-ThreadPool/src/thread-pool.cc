#include "thread-pool.h"
#include <stdexcept>

ThreadPool::ThreadPool(size_t numThreads)
    : workers(numThreads), shuttingDown(false), activeTasks(0)
{
    for (size_t i = 0; i < numThreads; ++i) {
        workers[i].taskReady = false;
        workers[i].available = true;
        
        workers[i].thread = std::thread(&ThreadPool::worker, this, i);
        workersAvailable.signal();
    }

    dispatcherThread = std::thread(&ThreadPool::dispatcher, this);
}

void ThreadPool::schedule(const std::function<void(void)>& task) {
    if (!task) throw std::invalid_argument("Tarea vacía no permitida");

    {
        std::lock_guard<std::mutex> lock(taskQueueMutex);
        if (shuttingDown) throw std::runtime_error("No se aceptan tareas, pool cerrándose");
        taskQueue.push(task);
    }

    {
        std::lock_guard<std::mutex> lock(activeTasksMutex);
        ++activeTasks;
    }

    tasksInQueue.signal(); // avisa al dispatcher
}

void ThreadPool::dispatcher() {
    while (true) {
        tasksInQueue.wait();

        std::function<void()> task;

        {
            std::lock_guard<std::mutex> lock(taskQueueMutex);
            if (shuttingDown && taskQueue.empty()) break;

            if (!taskQueue.empty()) {
                task = taskQueue.front();
                taskQueue.pop();
            } else {
                continue;
            }
        }

        workersAvailable.wait();

        size_t id = workers.size();
        {
            for (size_t i = 0; i < workers.size(); ++i) {
                if (workers[i].available) {
                    id = i;
                    break;
                }
            }
        }

        if (id == workers.size()) {
            workersAvailable.signal();
            {
                std::lock_guard<std::mutex> lock(taskQueueMutex);
                taskQueue.push(task); // reinsertar tarea si no había ningún worker libre
            }
            continue;
        }

        {
            std::lock_guard<std::mutex> lock(workerMutex);
            workers[id].task = task;
            workers[id].taskReady = true;
            workers[id].available = false;
        }

        workers[id].workerSem.signal();
    }

    // Enviar señales finales a todos los workers para que terminen
    for (auto& w : workers)
        w.workerSem.signal();
}

void ThreadPool::worker(size_t id) {
    while (true) {
        workers[id].workerSem.wait();

        std::function<void()> task;

        {
            std::lock_guard<std::mutex> lock(taskQueueMutex);
            if (shuttingDown && !workers[id].taskReady)
                break;
        }

        {
            std::lock_guard<std::mutex> lock(workerMutex);
            if (workers[id].taskReady) {
                task = workers[id].task;
                workers[id].task = nullptr;
                workers[id].taskReady = false;
            }
        }

        if (task)
            task();

        {
            std::lock_guard<std::mutex> lock(workerMutex);
            workers[id].available = true;
        }

        workersAvailable.signal();

        {
            std::lock_guard<std::mutex> lock(activeTasksMutex);
            if (--activeTasks == 0)
                allDoneCv.notify_all();
        }
    }
}

void ThreadPool::wait() {
    std::unique_lock<std::mutex> lock(activeTasksMutex);
    allDoneCv.wait(lock, [this]() { return activeTasks == 0; });
}

ThreadPool::~ThreadPool() {
    {
        std::lock_guard<std::mutex> lock(taskQueueMutex);
        shuttingDown = true;
    }

    tasksInQueue.signal();

    if (dispatcherThread.joinable())
        dispatcherThread.join();

    for (auto& w : workers)
        w.workerSem.signal();

    for (auto& w : workers)
        if (w.thread.joinable())
            w.thread.join();
}
