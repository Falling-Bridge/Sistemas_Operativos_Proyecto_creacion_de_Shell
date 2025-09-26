#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "Comando_personalizado.h"
#include "Comando_pipes.h"

void ejecutar_pipes(char *comandos[], int num_comandos);

int main(int argc, char *argv[])
{
    char escritura_comando[256]; // por la cantidad que admite un buffer
    while (1)
    {

        printf("\n$ ");
        fflush(stdout); // para mostrar el prompt

        if (!fgets(escritura_comando, 256, stdin))
        {
            break;
        }

        escritura_comando[strcspn(escritura_comando, "\n")] = 0;

        if (strlen(escritura_comando) == 0)
        {
            continue;
        }

        // para detectar si hay pipes
        char *pipe_token = strtok(escritura_comando, "|");
        char *comandos[10]; // 10 comandos pipe máximo.
        int num_comandos = 0;

        while (pipe_token != NULL && num_comandos < 10)
        {
            while (*pipe_token == ' ')
                pipe_token++;
            char *end = pipe_token + strlen(pipe_token) - 1;
            while (end > pipe_token && *end == ' ')
                end--;
            *(end + 1) = '\0';

            comandos[num_comandos++] = pipe_token;
            pipe_token = strtok(NULL, "|");
        }
        // comando exit
        if (strcmp(comandos[0], "exit") == 0)
        {
            break;
        }

        if (strcmp(comandos[0], "miprof") == 0)
        {
            manejar_miprof(comandos);
            continue;
        }

        // Para saber si ejecutar comando simple o pipes
        if (num_comandos == 1)
        {
            char *args[256];
            char *token = strtok(comandos[0], " ");
            int i = 0;

            while (token != NULL)
            {
                args[i] = token;
                token = strtok(NULL, " ");
                i++;
            }
            args[i] = NULL;
            if (args[0] == NULL)
            {
                continue;
            }
            pid_t pid = fork();
            if (pid < 0)
            {
                printf("Error en fork");
                exit(1);
            }
            else if (pid == 0)
            {
                execvp(args[0], args);
                perror("execvp error");
                exit(1);
            }
            else
            {
                wait(NULL);
            }
        }
        else
        {
            ejecutar_pipes(comandos, num_comandos);
        }
    }

    return 0;
}
