#include "Comando_personalizado.h"

double tiempo_usuario;
double tiempo_sistema;
double tiempo_real;

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
        printf("solo ejecutar");
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

    ejecutar_miprof(){
        return;
    }
    guardar_resultados(){
        return;
    }
  


    
}
