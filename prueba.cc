#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#define NUM_THREADS 10

int Var = 0;
pthread_mutex_t Mutex = PTHREAD_MUTEX_INITIALIZER;

void *ThreadFunc(void *Arg)
{
    pthread_mutex_lock(&Mutex);
    int Aux = Var + 1;
    usleep(1000);  // Simula un retardo
    Var = Aux;
    pthread_mutex_unlock(&Mutex);
    return NULL;
}

int main()
{
    int i;
    pthread_t ThreadId[NUM_THREADS];

    for (i = 0; i < NUM_THREADS; i++) {
        if (pthread_create(&ThreadId[i], NULL, ThreadFunc, NULL)) {
            printf("Error, imposible crear el nuevo hilo!\n");
            return -1;
        }
    }

    for (i = 0; i < NUM_THREADS; i++) {
        pthread_join(ThreadId[i], NULL);
    }

    printf("Fin de espera, saliendo con Var=%d.\n", Var);

    pthread_mutex_destroy(&Mutex);

    return 0;
}
