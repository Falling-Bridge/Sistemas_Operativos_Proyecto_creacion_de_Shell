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

#define MAX_TOKENS 20
#define MAX_LENGHT 256

typedef struct {
    double tiempo_real;
    double tiempo_usuario;
    double tiempo_sistema;
    long memoria_maxima;
} Resultados;

double get_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}

/* ejecutar_miprof
 *  - comandos: argv-like (comandos[0] comando, seguido de argumentos, NULL-terminated)
 *  - timeout: tiempo máximo (no usado actualmente)
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

    if (pid < 0) {
        perror("fork fallo\n");
        exit(1);
    } else if (pid == 0) {
        execvp(comandos[0], comandos);
        perror("execvp fallo");
        exit(1);
    }
    else {
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
    if (!f) { 
        perror("Error al abrir archivo para guardar resultados"); 
        return; 
    }
    fprintf(f, "--- Ejecución miprof ---\n");
    fprintf(f, "Comando: "); 
    for (int i = 0; args[i] != NULL; i++) 
        fprintf(f, "%s ", args[i]); 
    fprintf(f, "\n");
    fprintf(f, "Tiempo real: %.6f s\n", result.tiempo_real);
    fprintf(f, "Tiempo usuario: %.6f s\n", result.tiempo_usuario);
    fprintf(f, "Tiempo sistema: %.6f s\n", result.tiempo_sistema);
    fprintf(f, "Memoria máxima residente: %ld KB\n\n", result.memoria_maxima);
    fclose(f);
    printf("Resultados guardados en %s\n", archivo);
}

/* parsing_miprof
 * Parsea el string de comando completo en tokens separados por espacios
 * Retorna el número de tokens encontrados
 */
int parsing_miprof(char *comando_completo, char *tokens[MAX_TOKENS]) {
    int token_count = 0;
    char comando_copia[512];
    
    // Hacer copia porque strtok modifica el string
    strncpy(comando_copia, comando_completo, sizeof(comando_copia) - 1);
    comando_copia[sizeof(comando_copia) - 1] = '\0';
    
    // Tokenizar por espacios
    char *token = strtok(comando_copia, " ");
    
    while (token != NULL && token_count < MAX_TOKENS) {
        // Asignar el puntero directamente (apunta a comando_copia)
        // PROBLEMA: comando_copia es local, se perderá al salir
        // SOLUCIÓN: Usar un buffer estático o hacer copias
        
        // Opción 1: Usar buffer estático (más simple)
        static char buffer[MAX_TOKENS][MAX_LENGHT];
        strncpy(buffer[token_count], token, MAX_LENGHT - 1);
        buffer[token_count][MAX_LENGHT - 1] = '\0';
        tokens[token_count] = buffer[token_count];
        
        token_count++;
        token = strtok(NULL, " ");
    }
    
    tokens[token_count] = NULL;  // Terminar con NULL
    return token_count;
}

/* manejar_miprof
 *  - comandos: array donde comandos[0] contiene el string completo
 *              ejemplo: comandos[0] = "miprof ejec ls -la"
 *
 * Sintaxis admitida:
 *  - miprof ejec comando args
 *  - miprof ejecsave archivo comando args
 *  - miprof ejecutar <tiempo> comando args
 */
void manejar_miprof(char **comandos) {
    char *tokens[MAX_TOKENS];
    int num_tokens;
    
    // Parsear el comando completo que viene en comandos[0]
    num_tokens = parsing_miprof(comandos[0], tokens);
    
    if (num_tokens < 2) {
        printf("Uso: miprof <ejec|ejecsave archivo|ejecutar tiempo> comando args\n");
        return;
    }
    
    // tokens[0] = "miprof"
    // tokens[1] = modo (ejec, ejecsave, ejecutar)
    // tokens[2...] = resto de argumentos
    
    char *modo = tokens[1];
    
    if (strcmp(modo, "ejec") == 0) {
        // miprof ejec comando args
        if (tokens[2] == NULL) {
            printf("Debe especificar un comando\n");
            return;
        }
        
        Resultados result = ejecutar_miprof(&tokens[2], 0);
        
        // Mostrar resultados
        printf("\n=== Resultados de ejecución ===\n");
        printf("Tiempo real: %.6f s\n", result.tiempo_real);
        printf("Tiempo usuario: %.6f s\n", result.tiempo_usuario);
        printf("Tiempo sistema: %.6f s\n", result.tiempo_sistema);
        printf("Memoria máxima: %ld KB\n", result.memoria_maxima);
    }
    else if (strcmp(modo, "ejecsave") == 0) {
        // miprof ejecsave archivo comando args
        if (tokens[2] == NULL) {
            printf("Debe especificar archivo\n");
            return;
        }
        if (tokens[3] == NULL) {
            printf("Debe especificar comando\n");
            return;
        }
        
        char *archivo = tokens[2];
        Resultados result = ejecutar_miprof(&tokens[3], 0);
        guardar_resultados(archivo, &tokens[3], result);
        
        // También mostrar en pantalla
        printf("\n=== Resultados de ejecución ===\n");
        printf("Tiempo real: %.6f s\n", result.tiempo_real);
        printf("Tiempo usuario: %.6f s\n", result.tiempo_usuario);
        printf("Tiempo sistema: %.6f s\n", result.tiempo_sistema);
        printf("Memoria máxima: %ld KB\n", result.memoria_maxima);
    }
    else if (strcmp(modo, "ejecutar") == 0) {
        // miprof ejecutar <tiempo> comando args
        if (tokens[2] == NULL || tokens[3] == NULL) {
            printf("Uso correcto: miprof ejecutar <tiempo_max> comando args\n");
            return;
        }
        
        int timeout = atoi(tokens[2]);
        Resultados result = ejecutar_miprof(&tokens[3], timeout);
        
        // Mostrar resultados
        printf("\n=== Resultados de ejecución (timeout: %d s) ===\n", timeout);
        printf("Tiempo real: %.6f s\n", result.tiempo_real);
        printf("Tiempo usuario: %.6f s\n", result.tiempo_usuario);
        printf("Tiempo sistema: %.6f s\n", result.tiempo_sistema);
        printf("Memoria máxima: %ld KB\n", result.memoria_maxima);
    }
    else {
        printf("Modo no reconocido: %s\n", modo);
        printf("Modos válidos: ejec, ejecsave, ejecutar\n");
    }
}