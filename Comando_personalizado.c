#include "Comando_personalizado.h"

void manejar_miprof(char **comandos)
{
    if (comandos[1] == NULL)
    {
        printf("Uso: miprof <modo> [tiempo_maximo] <comando> [argumentos]\n");
        return;
    }

    char *modo = comandos[1];
    if (strcmp(modo, "ejec") == 0)
    {
        ejecutar_miprof(&comandos[2], 0);
    }
    else if(strcmp(modo, "ejecsave") == 0)
    {
        printf("ejecutar y guardar tiempo");
    }
    else if(strcmp(modo, "ejecutar") == 0)
    {
        // Modo: ejecutar con timeout
        if (comandos[2] == NULL || comandos[3] == NULL) {
            printf("Uso correcto: miprof ejecutar maxtiempo comando args\n");
            return 1;
        }
        printf("ejecutar en un tiempo maximo dado");
    }
    void guardar_resultados(){
        return;
    }
    
}

void ejecutar_miprof(char **comandos, int tiempo_maximo){
        struct rusage usage_start, usage_end;
        struct timespec real_start, real_end;
        double user_time, sys_time, real_time;

        double start_time = get_time();
        int pid = fork();

        if (pid < 0) { // fork falló
        perror("fork fallo\n");
        exit(1);
    } else if (pid == 0) { // proceso hijo
        execvp(comandos[0], &comandos[0]);
        perror("execvp fallo");
        exit(1);
        return;
    }
    else if(pid > 0) { // proceso padre

        wait4(pid, &status, 0, &usage_end);
        end_time = get_time();
    }
    else{//fork fallo
        perror("fork fallo");
        exit(1);
    }
    real_time = real_end.tv_sec - real_start.tv_sec + (real_end.tv_nsec - real_start.tv_nsec) / 1e9;
    user_time = (usage_end.ru_utime.tv_sec - usage_start.ru_utime.tv_sec) + (usage_end.ru_utime.tv_usec - usage_start.ru_utime.tv_usec) / 1e6;
    sys_time = (usage_end.ru_stime.tv_sec - usage_start.ru_stime.tv_sec) + (usage_end.ru_stime.tv_usec - usage_start.ru_stime.tv_usec) / 1e6;
    Resultados result = {user_time, sys_time, real_time};
    return result;
}
void mostrar_resultados(Resultados res) {
    printf("Tiempo de usuario: %.6f segundos\n", res.user_time);
    printf("Tiempo de sistema: %.6f segundos\n", res.sys_time);
    printf("Tiempo real: %.6f segundos\n", res.real_time);
}
