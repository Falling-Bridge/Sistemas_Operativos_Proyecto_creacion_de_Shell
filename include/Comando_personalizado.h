#ifndef COMANDO_PERSONALIZADO_H
#define COMANDO_PERSONALIZADO_H

#include "librerias.h"

typedef struct{
    double tiempo_usuario;
    double tiempo_sistema;
    double tiempo_real;
    long memoria_maxima;} Resultados;

void manejar_miprof(char **comandos);
Resultados ejecutar_miprof(char **args, int timeout);
void guardar_resultados(const char *archivo, char **args, Resultados result);

#endif