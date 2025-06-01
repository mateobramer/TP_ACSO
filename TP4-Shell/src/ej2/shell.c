#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define MAX_COMMANDS 200
#define MAX_ARGS 64

int main() {

    char command[256];
    char *commands[MAX_COMMANDS];
    int command_count;

    while (1) 
    {
        printf("Shell> ");
        fflush(stdout);

        if (!fgets(command, sizeof(command), stdin)) break;

        command[strcspn(command, "\n")] = '\0';

        // Salir del shell
        if (strcmp(command, "exit") == 0) break;

        // Separar por pipes
        command_count = 0;
        char *token = strtok(command, "|");
        while (token != NULL) 
        {
            commands[command_count++] = token;
            token = strtok(NULL, "|");
        }

        int prev_fd = -1;
        int pipefd[2];

        for (int i = 0; i < command_count; i++) 
        {
            // Separar argumentos del comando
            char *args[MAX_ARGS];
            int arg_count = 0;

            char *arg = strtok(commands[i], " \t");
            while (arg != NULL) {
                if (arg_count >= MAX_ARGS ) {
                    fprintf(stderr, "Error: demasiados argumentos\n");
                    break; 
    }
                args[arg_count++] = arg;
                arg = strtok(NULL, " \t");
            }
            args[arg_count] = NULL;

            // Crear pipe si no es el último
            if (i < command_count - 1) {
                if (pipe(pipefd) == -1) {
                    perror("pipe");
                    exit(1);
                }
            }

            pid_t pid = fork();
            if (pid == -1) {
                perror("fork");
                exit(1);
            }

            if (pid == 0) {
                // Redirigir stdin si no es el primer comando
                if (i > 0) {
                    dup2(prev_fd, STDIN_FILENO);
                    close(prev_fd);
                }

                // Redirigir stdout si no es el último comando
                if (i < command_count - 1) {
                    close(pipefd[0]);
                    dup2(pipefd[1], STDOUT_FILENO);
                    close(pipefd[1]);
                }

                execvp(args[0], args);
                perror("execvp");
                exit(1);
            }

            // PADRE
            if (i > 0) close(prev_fd);  // Cerramos el anterior
            if (i < command_count - 1) {
                close(pipefd[1]);       // Cerramos escritura
                prev_fd = pipefd[0];    // Guardamos lectura para el próximo
            }
        }

        // Esperar a todos los hijos
        for (int i = 0; i < command_count; i++) {
            wait(NULL);
        }
    }

    return 0;
}
