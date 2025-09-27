#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

char *myargs[3];

int main(int argc, char *argv[]) {
    int pid = fork();
    if (pid < 0) { // fork fallo
        printf("fork fallo\n");
        exit(1);
    } else if (pid == 0) { //hijo
        printf("Soy hijo H: %d mi padre P : %d\n", getpid(), getppid());
        myargs[0] = argv[1];   // comando dado por linea de comando 
        myargs[1] = argv[2]; // argumento de comando dado en linea de comando 
        myargs[2] = argv[3]; 
        myargs[3] = NULL;        // no mas argumentos para el comando
        execvp(myargs[0], myargs);  // cambia imagen de hijo al comando  
        printf("Aqui llegaría solo ante el error de execvp\n");
    } else { // padre
        int res = wait(NULL);//espera por su hijo
        printf("Soy padre P: %d mi mi Hijo : %d o res %d \n", getpid(), pid, res);//imprime info
    }
    printf("FIN proc actual %d\n", getpid());
    return 0;
}