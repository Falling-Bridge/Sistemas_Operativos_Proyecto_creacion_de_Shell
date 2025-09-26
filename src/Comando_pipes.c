#include "../include/Comando_pipes.h"

/*
 * ejecutar_pipes
 * ----------------
 * Ejecuta un pipeline de comandos separados por '|'.
 *
 * comandos: array de cadenas (cada elemento es un comando con sus args separados por espacios)
 * num_comandos: cantidad de elementos válidos en `comandos`
 *
 * Para cada comando:
 * - Crea un proceso hijo con fork
 * - Redirige stdin y stdout según corresponda
 * - Ejecuta el comando con execvp
 * - El padre espera a todos los hijos
 */
void ejecutar_pipes(char *comandos[], int num_comandos)
{
    int i;
    int in_fd = 0; // stdin
    int fd[2];
    pid_t pid;

    for (i = 0; i < num_comandos; i++)
    {
        // crear pipes para todo menos para el último comando
        if (i < num_comandos - 1)
        {
            if (pipe(fd) < 0)
            {
                perror("error al ejecutar pipe");
                return;
            }
        }

        pid = fork();
        if (pid == 0) // hijo
        {
            if (i > 0)
            {
                // Redirige entrada desde el pipe anterior
                dup2(in_fd, STDIN_FILENO);
                close(in_fd);
            }
            if (i < num_comandos - 1)
            {
                // Redirige entrada al pipe siguiente
                dup2(fd[1], STDOUT_FILENO);
                close(fd[1]);
                close(fd[0]);
            }

            /* Parsear argumentos */
            char *args[256];
            char *token = strtok(comandos[i], " ");
            int j = 0;
            while (token != NULL)
            {
                args[j++] = token;
                token = strtok(NULL, " ");
            }
            args[j] = NULL;

            execvp(args[0], args);
            perror("error execvp");
            exit(1);
        }
        else if (pid > 0) // padre
        {
            if (i > 0) close(in_fd);
            if (i < num_comandos - 1)
            {
                close(fd[1]);
                in_fd = fd[0]; // entrada para el proximo cmdo
            }
        }
        else
        {
            perror("Error en el fork");
            return;
        }
    }

    /* Esperar todos los procesos hijos */
    for (i = 0; i < num_comandos; i++) wait(NULL);
}