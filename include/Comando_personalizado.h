#ifndef COMANDO_PERSONALIZADO_H
#define COMANDO_PERSONALIZADO_H

#include "librerias.h"

void manejar_miprof(char **comandos);
void ejecutar_miprof(char **args, int timeout, int guardar, const char *archivo);
void guardar_resultados(const char *archivo, char **args);

#endif