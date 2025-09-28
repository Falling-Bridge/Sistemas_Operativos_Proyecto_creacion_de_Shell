#include "../include/Comando_personalizado.h"

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

/* Variables globales para almacenar métricas de ejecución (placeholder) */
double tiempo_usuario;
double tiempo_sistema;
double tiempo_real;
long memoria_maxima;

void guardar_resultados(const char *archivo, char **args);

/* ejecutar_miprof
 *  - args: argv-like (args[0] comando, seguido de argumentos, NULL-terminated)
 *  - timeout: tiempo máximo (no usado actualmente)
 *  - guardar: flag para indicar si hay que guardar resultados
 *  - archivo: ruta del archivo donde guardar si guardar != 0
 *
 * Implementación actual: simplemente fork/exec del comando y espera.
 */
void ejecutar_miprof(char **args, int timeout, int guardar, const char *archivo) {
     /* parámetros no usados por ahora */
    (void)timeout; 
    (void)guardar; 
    (void)archivo;
    pid_t pid = fork();
    if (pid == 0) {
        execvp(args[0], args);
        perror("execvp error");
        exit(1);
    } else if (pid > 0) {
        int status;
        waitpid(pid, &status, 0);
    } else {
        perror("fork error");
    }
}

void guardar_resultados(const char *archivo, char **args) {
    FILE *f = fopen(archivo, "a");
    if (!f) { perror("Error al abrir archivo para guardar resultados"); return; }
    fprintf(f, "--- Ejecución miprof ---\n");
    fprintf(f, "Comando: "); for (int i = 0; args[i] != NULL; i++) fprintf(f, "%s ", args[i]); fprintf(f, "\n");
    fprintf(f, "Tiempo real: %.6f s\n", tiempo_real);
    fprintf(f, "Tiempo usuario: %.6f s\n", tiempo_usuario);
    fprintf(f, "Tiempo sistema: %.6f s\n", tiempo_sistema);
    fprintf(f, "Memoria máxima residente: %ld KB\n\n", memoria_maxima);
    fclose(f);
    printf("Resultados guardados en %s\n", archivo);
    (void)args; (void)archivo;
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
    if (comandos[1] == NULL) { printf("Uso: miprof <ejec|ejecsave archivo|ejecutar tiempo> comando args\n"); return; }
    char *modo = comandos[1];

    if (strcmp(modo, "ejec") == 0) {
        // miprof ejec comando args
        ejecutar_miprof(&comandos[2], 0, 0, NULL);
    }
    else if (strcmp(modo, "ejecsave") == 0) {
        // miprof ejecsave archivo comando args
        if (comandos[2] == NULL) {
            printf("Debe especificar archivo\n");
            return;
        }
        char *archivo = comandos[2];
        ejecutar_miprof(&comandos[3], 0, 1, archivo);
    }
    else if (strcmp(modo, "ejecutar") == 0) {
        // miprof ejecutar <tiempo> comando args
        if (comandos[2] == NULL || comandos[3] == NULL) {
            printf("Uso correcto: miprof ejecutar <tiempo_max> comando args\n");
            return;
        }
        int timeout = atoi(comandos[2]);
        ejecutar_miprof(&comandos[3], timeout, 0, NULL);
    }
    else {
        printf("Modo no reconocido: %s\n", modo);
    }
}