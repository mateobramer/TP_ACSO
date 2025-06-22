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

#include <cstddef>     // for size_t
#include <functional>  // for the function template used in the schedule signature
#include <thread>      // for thread
#include <vector>      // for vector
#include "Semaphore.h" // for Semaphore
#include <queue>
#include <mutex>
#include <condition_variable>

using namespace std;

class ThreadPool {
  public:

  /**
  * Constructs a ThreadPool configured to spawn up to the specified
  * number of threads.
  */
    ThreadPool(size_t numThreads);

  /**
  * Schedules the provided thunk (which is something that can
  * be invoked as a zero-argument function without a return value)
  * to be executed by one of the ThreadPool's threads as soon as
  * all previously scheduled thunks have been handled.
  */
    void schedule(const function<void(void)>& thunk);

  /**
  * Blocks and waits until all previously scheduled thunks
  * have been executed in full.
  */
    void wait();

  /**
  * Waits for all previously scheduled thunks to execute, and then
  * properly brings down the ThreadPool and any resources tapped
  * over the course of its lifetime.
  */
    ~ThreadPool();
    
private:

  class Worker {
      public:
          Worker(size_t id, ThreadPool* pool) : workerId(id), threadPool(pool) {}
          // Este operador lanza la ejecución del worker
          void operator()() {
              threadPool->worker(workerId);
          }

      private:
          size_t workerId;
          ThreadPool* threadPool;
      };



    void dispatcher();
    void worker(size_t id);

    // Hilos de workers
    std::vector<std::thread> workerThreads;

    // Hilo dispacher
    std::thread dispatcherThread;

    // Cola de tareas por programar (entrada)
    std::queue<std::function<void(void)>> taskQueueInput;

    // Cola de tareas listas para ejecutar (salida hacia workers)
    std::queue<std::function<void(void)>> taskQueueReady;

    // Cantidad de workers y tareas ekecutándose
    size_t totalWorkers;
    size_t activeTaskCount;

    // Mutexes y condición para sincronización
    std::mutex taskQueueMutex;         // Protege las dos colas
    std::mutex activeCountMutex;       // Protege activeTaskCount
    std::condition_variable allTasksDoneCV; // Usado en wait()

    // Semáforos
    Semaphore tasksInQueue;          // Cantidad de tareas en cola de entrada 
    Semaphore spaceInReadyQueue;       // Capacidad libre para pasar tareas 
    Semaphore readyTasksAvailable;     // Tareas listas 

    // Flag de terminación del pool
    bool shuttingDown = false;

    // Evita copia accidental del ThreadPool
    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;
};

#endif // _THREAD_POOL_H_
