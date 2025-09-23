#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    char escritura_comando[256]; // por la cantidad que admite un buffer
    while (1)
    {
        printf("\n$ ");
        fgets(escritura_comando, 256, stdin);
        escritura_comando[strcspn(escritura_comando, "\n")] = 0;

        char *args[256];
        char *token = strtok(escritura_comando, " ");

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
            continue; // Si no se ingresó nada, vuelve al inicio del bucle.
        }

        if (strcmp(args[0], "exit") == 0)
            break;

        int pid = fork();
        if (pid < 0)
        { // fork fallo
            printf("fork fallo\n");
            exit(1);
        }
        else if (pid == 0)
        {
            execvp(args[0], args);
        }
        else
        { // padre
            int res = wait(NULL);
        }
        // printf("FIN proc actual %d\n", getpid());
    }

    return 0;
}
