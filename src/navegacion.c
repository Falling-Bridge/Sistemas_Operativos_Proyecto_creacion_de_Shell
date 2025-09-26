/*
 * navegacion.c
 * Funciones relacionadas con la navegación y el autocompletado:
 * - Comparación case-insensitive
 * - Parseo de rutas para el autocompletado
 * - Implementación del completador usado por readline
 * - Procesamiento del comando `cd` y variantes como `cd..`
 *
 */

#include "../include/navegacion.h"

/* Comparación de cadenas sin distinguir mayúsculas/minúsculas.
 * Devuelve el mismo resultado que strcasecmp.
 */
int str_case_cmp(const char *s1, const char *s2) { return strcasecmp(s1, s2); }

void parse_path(const char *text, char *dir_path, size_t dir_size, char *pattern, size_t pat_size)
{
    const char *p = strrchr(text, '/');
    if (!p)
    {
        strncpy(dir_path, ".", dir_size);
        strncpy(pattern, text, pat_size - 1);
    }
    else
    {
        size_t n = p - text;
        if (n >= dir_size)
            n = dir_size - 1;
        memcpy(dir_path, text, n);
        dir_path[n] = '\0';
        snprintf(pattern, pat_size, "%s", p + 1);
        if (dir_path[0] == '\0')
            strncpy(dir_path, "/", dir_size);
    }
    dir_path[dir_size - 1] = pattern[pat_size - 1] = '\0';
}

/*
 * completer_func
 *  - text: texto parcial a completar (puede contener '/' para directorios)
 *  - state: estado interno de readline (0 la primera llamada para este texto)
 *
 * Esta función explora el directorio correspondiente (extraído por
 * parse_path) y devuelve cadenas duplicadas con las posibles
 * coincidencias. Readline liberará las cadenas.
 */
char *completer_func(const char *text, int state)
{
    static DIR *d = NULL;
    static struct dirent *e;
    static char pat[256], dpath[256];
    static char buf[512];
    if (state == 0)
    {
        if (d)
        {
            closedir(d);
            d = NULL;
        }
        parse_path(text, dpath, sizeof(dpath), pat, sizeof(pat));
        d = opendir(dpath);
        if (!d)
        {
            strncpy(dpath, ".", sizeof(dpath));
            strncpy(pat, text, sizeof(pat) - 1);
            d = opendir(".");
        }
        if (!d)
            return NULL;
        rewinddir(d);
    }
    while ((e = readdir(d)))
    {
        if (pat[0] != '.' && e->d_name[0] == '.')
            continue;
        if (strncasecmp(e->d_name, pat, strlen(pat)) == 0)
        {
            if (strcmp(dpath, ".") == 0)
                snprintf(buf, sizeof(buf), "%s", e->d_name);
            else
                snprintf(buf, sizeof(buf), "%s/%s", dpath, e->d_name);
            if (e->d_type == DT_DIR)
                strncat(buf, "/", sizeof(buf) - strlen(buf) - 1);
            return strdup(buf);
        }
    }
    if (d)
    {
        closedir(d);
        d = NULL;
    }
    return NULL;
}

/*
 * my_completion
 *  - configuración de readline para que use `completer_func`
 *  - mantiene comportamiento: añadir '/' a directorios, suprimir comillas
 */
char **my_completion(const char *text, int start, int end)
{
    (void)start;
    (void)end;
    rl_completion_append_character = '/';
    rl_completion_suppress_append = 0;
    rl_completion_quote_character = '\0';
    rl_completion_suppress_quote = 1;
    rl_bind_key('\t', rl_complete);
    return rl_completion_matches(text, completer_func);
}

/*
 * procesar_cd
 *  - args: vector tipo argv donde args[0] == "cd" y args[1] el destino
 *
 * Reglas de navegación (consejos ya existentes):
 *  - "cd .." mueve al padre
 *  - no se permiten rutas absolutas (empiezan por '/')
 *  - no se permiten rutas que contengan ".." en medio
 */
void procesar_cd(char *args[])
{
    if (!args[1])
    {
        printf("Uso: cd <nombre_carpeta> o cd ..\n");
        return;
    }
    if (strcmp(args[1], "..") == 0)
    {
        if (chdir("..") != 0)
            perror("cd error");
        return;
    }
    if (args[1][0] == '/')
    {
        printf("No se permiten rutas absolutas\n");
        return;
    }
    if (strstr(args[1], "..") != NULL)
    {
        printf("No se permiten rutas con '..' en medio\n");
        return;
    }
    if (chdir(args[1]) != 0)
        perror("cd error");
}

// Normaliza y maneja comandos de navegación escritos en la línea.
// Devuelve 1 si la línea fue tratada como navegación (y ya se ejecutó), 0 en caso contrario.
/*
 * manejar_navegacion_de_linea
 *  - Normaliza variantes de `cd..` a `cd ..` y, si la línea empieza por
 *    `cd`, la parsea y delega la acción a `procesar_cd`.
 *  - Retorna 1 si la línea fue tratada como navegación (ya ejecutada),
 *    o 0 si no corresponde a un comando de navegación.
 */
int manejar_navegacion_de_linea(char *line)
{
    if (!line || line[0] == '\0')
        return 0;

    // Si la línea empieza por "cd.." la normalizamos a "cd .." y conservamos el resto
    if (strncmp(line, "cd..", 4) == 0)
    {
        char tmp[256];
        snprintf(tmp, sizeof(tmp), "cd ..%s", line + 4);
        strncpy(line, tmp, strlen(tmp) + 1);
    }

    // Si la línea ahora empieza por "cd" la separamos y delegamos a procesar_cd
    if (strncmp(line, "cd", 2) == 0 && (line[2] == ' ' || line[2] == '\t' || line[2] == '\0'))
    {
        char copy[256];
        strncpy(copy, line, sizeof(copy) - 1);
        copy[sizeof(copy) - 1] = '\0';
        char *token = strtok(copy, " \t");
        char *args[3];
        int i = 0;
        while (token != NULL && i < 2)
        {
            args[i++] = token;
            token = strtok(NULL, " \t");
        }
        args[i] = NULL;
        procesar_cd(args);
        return 1;
    }

    return 0;
}