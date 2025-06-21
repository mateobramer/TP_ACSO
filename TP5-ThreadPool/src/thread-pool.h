#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_

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


    /**
     * @brief Hilo que reparte tareas desde la cola de entrada a los workers disponibles.
     */
    void dispatcher();

    /**
     * @brief Función que ejecuta cada hilo worker.
     * @param id Identificador del worker.
     */
    void worker(size_t id);

    // Hilos de workers
    std::vector<std::thread> workerThreads;

    // Hilo dispatcher
    std::thread dispatcherThread;

    // Cola de tareas por programar (entrada)
    std::queue<std::function<void(void)>> taskQueueInput;

    // Cola de tareas listas para ejecutar (salida hacia workers)
    std::queue<std::function<void(void)>> taskQueueReady;

    // Cantidad de workers y tareas ejecutándose
    size_t totalWorkers;
    size_t activeTaskCount;

    // Mutexes y condición para sincronización
    std::mutex taskQueueMutex;         // Protege las dos colas
    std::mutex activeCountMutex;       // Protege activeTaskCount
    std::condition_variable allTasksDoneCV; // Usado en wait()

    // Semáforos
    Semaphore tasksAvailable;          // Cantidad de tareas en cola de entrada 
    Semaphore spaceInReadyQueue;       // Capacidad libre para pasar tareas 
    Semaphore readyTasksAvailable;     // Tareas listas 

    // Flag de terminación del pool
    bool shuttingDown = false;

    // Evita copia accidental del ThreadPool
    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;
};

#endif // _THREAD_POOL_H_
