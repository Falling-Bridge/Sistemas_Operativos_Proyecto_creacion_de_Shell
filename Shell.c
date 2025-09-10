#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    int pid = fork();
    if (pid < 0)
    { // fork fallo
        printf("fork fallo\n");
        exit(1);
    }
    else if (pid == 0)
    {                    // Este ejemplo es de una ayudantia, pero se puede ocupar como base para poder avanzar
        char *myargs[4]; // arreglo para almacenar el programa y sus argumentos
        myargs[0] = strdup("wc");
        myargs[1] = strdup("-l");
        myargs[2] = strdup("fork0.c");
        myargs[3] = NULL;
        execvp(myargs[0], myargs); // cambia imagen de hijo a wc
        printf("Aqui llegaría solo ante el error de execvp\n");
    }
    else
    { // padre
        int res = wait(NULL);
    }
    printf("FIN proc actual %d\n", getpid());
    return 0;
}
