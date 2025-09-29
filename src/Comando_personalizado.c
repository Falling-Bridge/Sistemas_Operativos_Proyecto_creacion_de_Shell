#include "../include/Comando_personalizado.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <signal.h>
#include <errno.h>

/*
 * Comando_personalizado.c
 * Implementaciones para el comando `miprof` usado en el shell.
 * - ejecutar_miprof: ejecuta un comando (posible extensión para medir tiempos/recursos)
 * - guardar_resultados: escribe métricas en un archivo
 * - manejar_miprof: interprete de la sub-sintaxis `miprof ...`
 *
 * Actualmente las mediciones (tiempo, memoria) están preparadas mediante
 * variables globales; la función ejecutar_miprof puede ampliarse para medir
 * y rellenar esas variables.
 */

       
double get_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}

/* ejecutar_miprof
 *  - args: argv-like (args[0] comando, seguido de argumentos, NULL-terminated)
 *  - timeout: tiempo máximo (no usado actualmente)
 *  - guardar: flag para indicar si hay que guardar resultados
 *  - archivo: ruta del archivo donde guardar si guardar != 0
 *
 * Implementación actual: simplemente fork/exec del comando y espera.
 */
Resultados ejecutar_miprof(char **comandos, int timeout){
    Resultados result = {0};
    (void)timeout; 

    struct rusage usage;
    int status;

    double user_time, sys_time, real_time;

    double start_time = get_time();
    double end_time;


    int pid = fork();

    if (pid < 0) { // fork falló
        perror("fork fallo\n");
        exit(1);
    } else if (pid == 0) { // proceso hijo
        execvp(comandos[0], comandos);
        perror("execvp fallo");
        exit(1);
    }
    else { // proceso padre
        wait4(pid, &status, 0, &usage);
        end_time = get_time();
    }
   
    real_time = end_time - start_time;  
    user_time = usage.ru_utime.tv_sec + usage.ru_utime.tv_usec / 1000000.0;
    sys_time = usage.ru_stime.tv_sec + usage.ru_stime.tv_usec / 1000000.0;

    result.tiempo_usuario = user_time;
    result.tiempo_sistema = sys_time;
    result.tiempo_real = real_time;
    result.memoria_maxima = usage.ru_maxrss;

    return result;
}


void guardar_resultados(const char *archivo, char **args, Resultados result) {
    FILE *f = fopen(archivo, "a");
    if (!f) { perror("Error al abrir archivo para guardar resultados"); return; }
    fprintf(f, "--- Ejecución miprof ---\n");
    fprintf(f, "Comando: "); for (int i = 0; args[i] != NULL; i++) fprintf(f, "%s ", args[i]); fprintf(f, "\n");
    fprintf(f, "Tiempo real: %.6f s\n", result.tiempo_real);
    fprintf(f, "Tiempo usuario: %.6f s\n", result.tiempo_usuario);
    fprintf(f, "Tiempo sistema: %.6f s\n", result.tiempo_sistema);
    fprintf(f, "Memoria máxima residente: %ld KB\n\n", result.memoria_maxima);
    fclose(f);
    printf("Resultados guardados en %s\n", archivo);
}

/* manejar_miprof
 *  - comandos: array de cadenas donde comandos[0]=="miprof"
 *
 * Sintaxis admitida:
 *  - miprof ejec comando args
 *  - miprof ejecsave archivo comando args
 *  - miprof ejecutar <tiempo> comando args
 */
void manejar_miprof(char **comandos) {
     /* Parsear argumentos */
    printf("DEBUG: args[0] = '%s'\n", comandos[0]);

    if (comandos[1] == NULL) { printf("Uso: miprof <ejec|ejecsave archivo|ejecutar tiempo> comando args\n"); return; }
    char *modo = comandos[1];

    if (strcmp(modo, "ejec") == 0) {
        // miprof ejec comando args
        ejecutar_miprof(&comandos[2], 0);
    }
    else if (strcmp(modo, "ejecsave") == 0) {
        // miprof ejecsave archivo comando args
        if (comandos[2] == NULL) {
            printf("Debe especificar archivo\n");
            return;
        }
        char *archivo = comandos[2];
        Resultados result = ejecutar_miprof(&comandos[3], 0);
        guardar_resultados(archivo, &comandos[3], result);
    }
    else if (strcmp(modo, "ejecutar") == 0) {
        // miprof ejecutar <tiempo> comando args
        if (comandos[2] == NULL || comandos[3] == NULL) {
            printf("Uso correcto: miprof ejecutar <tiempo_max> comando args\n");
            return;
        }
        int timeout = atoi(comandos[2]);
        ejecutar_miprof(&comandos[3], timeout);
    }
    else {
        printf("Modo no reconocido: %s\n", modo);
    }
}