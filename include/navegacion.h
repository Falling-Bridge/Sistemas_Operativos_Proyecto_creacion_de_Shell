#ifndef NAVEGACION_H
#define NAVEGACION_H

#include "librerias.h"

// Comparación de cadenas sin distinguir mayúsculas/minúsculas
int str_case_cmp(const char *s1, const char *s2);

// Función para separar la ruta del directorio y el patrón de búsqueda
void parse_path(const char *text, char *dir_path, size_t dir_size, char *pattern, size_t pat_size);

// Función para completar nombres de archivos y directorios
char* completer_func(const char* text, int state);

// Integración con readline para autocompletado en la línea de comandos
char** my_completion(const char *text, int start, int end);

// Función para procesar el comando cd
void procesar_cd(char *args[]);

// Detecta y maneja comandos de navegación leídos en la línea (p. ej. "cd", "cd..", "cd ../ruta").
// Retorna 1 si la línea fue manejada como navegación (y ya se ejecutó), 0 en caso contrario.
int manejar_navegacion_de_linea(char *line);

#endif
