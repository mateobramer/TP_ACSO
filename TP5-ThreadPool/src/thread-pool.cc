#include "thread-pool.h"
#include <iostream>
#include <stdexcept>

// Constructor del ThreadPool
ThreadPool::ThreadPool(size_t numThreads)
    : workerThreads(numThreads),  // vector de threads con tamaño fijo
      totalWorkers(numThreads),
      activeTaskCount(0),
      tasksInQueue(0),
      spaceInReadyQueue(numThreads),
      readyTasksAvailable(0),
      shuttingDown(false)
{
    // Iniciar hilo dispatcher
    dispatcherThread = std::thread([this]() { dispatcher(); });

    // Crear cada worker thread y asignarlo por índice
    for (size_t i = 0; i < numThreads; i++) {
        workerThreads[i] = std::thread([this, i]() { worker(i); });
    }
}


void ThreadPool::schedule(const std::function<void(void)>& tarea) {
    // Verificamos que la tarea no sea nula
    if (!tarea) {
        throw std::invalid_argument("schedule: tarea vacía no permitida");
    }

    // Se incrementa el contador de tareas activas
    activeCountMutex.lock();
    if (shuttingDown) {
        activeCountMutex.unlock();
        throw std::runtime_error("ThreadPool se está cerrando, no se pueden agregar tareas");
    }
    activeTaskCount++;
    activeCountMutex.unlock();

    // Se agrega la tarea a la cola de entrada (taskQueueInput)
    taskQueueMutex.lock();
    taskQueueInput.push(tarea);
    taskQueueMutex.unlock();

    // Se avisa al dispatcher que hay una nueva tarea
    tasksInQueue.signal();
}

void ThreadPool::wait() {
    activeCountMutex.lock();

    // Esperamos hasta que no haya más tareas activas
    while (activeTaskCount != 0) {
        std::unique_lock<std::mutex> lock(activeCountMutex, std::adopt_lock);
        allTasksDoneCV.wait(lock, [this] { return activeTaskCount == 0; });
        lock.release();  // Para que no desbloquee el mutex automáticamente al salir
    }

    activeCountMutex.unlock();
}

void ThreadPool::dispatcher() {
    while (true) {
        // Esperamos a que haya tareas en taskQueueInput
        tasksInQueue.wait();

        std::function<void(void)> tarea;

        // Accesemos a la cola de entrada taskQueueInput
        taskQueueMutex.lock();
        if (shuttingDown && taskQueueInput.empty()) {
            taskQueueMutex.unlock();
            break;  // Si estamos cerrando y no hay tareas, salir
        }
        if (taskQueueInput.empty()) {
            taskQueueMutex.unlock();
            continue; // Nada que hacer
        }

        // Tomamos ls tarea de taskQueueInput
        tarea = taskQueueInput.front();
        taskQueueInput.pop();
        taskQueueMutex.unlock();

        // Esperamos espacio libre en la cola de tareas listas (taskQueueReady)
        spaceInReadyQueue.wait();

        // Insertamos la tarea en la cola taskQueueReady
        taskQueueMutex.lock();
        taskQueueReady.push(tarea);
        taskQueueMutex.unlock();

        // Avisamos a un worker que hay tarea lista
        readyTasksAvailable.signal();
    }
}


void ThreadPool::worker(size_t id) {
    while (true) {
        // Esperamos a que haya tareas kistas en taskQueueReady
        readyTasksAvailable.wait();

        std::function<void(void)> tarea;

        // Accedemos a la cola taskQueueReady
        taskQueueMutex.lock();
        if (shuttingDown && taskQueueReady.empty()) {
            taskQueueMutex.unlock();
            break;  // Si estamos cerrando y no hay tareas, salir
        }
        if (taskQueueReady.empty()) {
            taskQueueMutex.unlock();
            continue;  // Nada que hacer
        }

        // Tomamos la tarea lista
        tarea = taskQueueReady.front();
        taskQueueReady.pop();
        taskQueueMutex.unlock();

        // Ejecutamos la tarea
        tarea();

        // Libetamos espacio en la cola taskQueueReady
        spaceInReadyQueue.signal();

        // Actualizamos el contador de tareas activas
        activeCountMutex.lock();
        activeTaskCount--;
        if (activeTaskCount == 0) {
            allTasksDoneCV.notify_all();  // Notifricamos que se terminó todo
        }
        activeCountMutex.unlock();
    }
}


ThreadPool::~ThreadPool() {
    // Indicamos que no se aceptan más tareas
    activeCountMutex.lock();
    shuttingDown = true;
    activeCountMutex.unlock();

    // Desbloqueamos el dispatcher (si está esperando)
    tasksInQueue.signal();
    if (dispatcherThread.joinable()) {
        dispatcherThread.join();  // Esperamos que termine
    }

    // Desbloqueamos los workers (si están esperando)
    for (size_t i = 0; i < totalWorkers; ++i) {
        readyTasksAvailable.signal();
    }

    // Esperamos a que terminen todos los workers
    for (auto& wt : workerThreads) {
        if (wt.joinable()) {
            wt.join();
        }
    }
}

