#include "../include/navegacion.h"
#include "../include/Comando_personalizado.h"

/* Shell.c
 * Programa principal del shell educativo.
 * - Inicializa readline y autocompletado
 * - Lee líneas de comando y las ejecuta
 * - Delegación: navegación (cd) -> navegacion.c
 * - Ejecución simple con fork/exec y soporte para pipes
 */

void ejecutar_pipes(char *comandos[], int num_comandos);

int main(int argc, char *argv[])
{
    (void)argc; (void)argv;
    char escritura_comando[256], comando_copia[256], cwd[1024], prompt[1100];
    /* Configurar autocompletado */
    rl_attempted_completion_function = my_completion;
    rl_bind_key('\t', rl_menu_complete);
    rl_completion_append_character = '/';
    rl_completion_suppress_append = 0;

    while (1)
    {
        /* Construir prompt con directorio actual */
        if (getcwd(cwd, sizeof(cwd)) != NULL)
            snprintf(prompt, sizeof(prompt), "%s $ ", cwd);
        else
        {
            perror("getcwd error");
            strcpy(prompt, "$ ");
        }
        char *input = readline(prompt);
        if (!input) break; /* EOF */
        if (strlen(input) > 0) add_history(input);
        strncpy(escritura_comando, input, sizeof(escritura_comando) - 1);
        escritura_comando[sizeof(escritura_comando) - 1] = '\0';
        free(input);

        /* Delegar la navegación a navegacion.c; si retorna 1 ya se ejecutó */
        if (manejar_navegacion_de_linea(escritura_comando)) continue;

        if (strlen(escritura_comando) == 0) { fflush(stdout); continue; }

        /* Separar por pipes y limpiar espacios alrededor */
        strncpy(comando_copia, escritura_comando, sizeof(comando_copia) - 1);
        comando_copia[sizeof(comando_copia) - 1] = '\0';
        char *pipe_token = strtok(escritura_comando, "|");
        char *comandos[10]; int num_comandos = 0;
        while (pipe_token != NULL && num_comandos < 10) {
            while (*pipe_token == ' ') pipe_token++;
            char *end = pipe_token + strlen(pipe_token) - 1;
            while (end > pipe_token && *end == ' ') end--;
            *(end + 1) = '\0';
            comandos[num_comandos++] = pipe_token;
            pipe_token = strtok(NULL, "|");
        }
        if (num_comandos == 0) continue;
        if (strcmp(comandos[0], "exit") == 0) break;
        if (strncmp(comandos[0], "miprof", 6) == 0) { manejar_miprof(comandos); continue; }

        /* Ejecutar comando simple o pipeline */
        if (num_comandos == 1) {
            /* reconstruir argv desde copia original */
            char *args_original[256]; char *token = strtok(comando_copia, " "); int i = 0;
            while (token != NULL && i < 255) { args_original[i++] = token; token = strtok(NULL, " "); }
            args_original[i] = NULL;
            if (args_original[0] == NULL) continue;

            pid_t pid = fork();
            if (pid < 0)
            {
                perror("fork error");
                exit(1);
            }
            else if (pid == 0)
            {
                execvp(args_original[0], args_original);
                perror("execvp error");
                exit(1);
            }
            else
                wait(NULL);
        }
        else
            ejecutar_pipes(comandos, num_comandos);
        }
    }

    return 0;
}

/* ejecutar_pipes
 *  - comandos: array de cadenas (cada elemento es un comando con sus args separados por espacios)
 *  - num_comandos: cantidad de elementos válidos en `comandos`
 *
 * Implementa la creación de pipes y procesos para ejecutar una tubería de comandos.
 */
void ejecutar_pipes(char *comandos[], int num_comandos)
{
    int i, in_fd = 0, fd[2];
    pid_t pid;
    for (i = 0; i < num_comandos; i++)
    {
        if (i < num_comandos - 1)
            if (pipe(fd) < 0)
            {
                perror("pipe error");
                return;
            }
        pid = fork();
        if (pid == 0)
        {
            if (i > 0)
            {
                dup2(in_fd, STDIN_FILENO);
                close(in_fd);
            }
            if (i < num_comandos - 1)
            {
                dup2(fd[1], STDOUT_FILENO);
                close(fd[1]);
                close(fd[0]);
            }
            char *args[256];
            char cmdcpy[256];
            strncpy(cmdcpy, comandos[i], sizeof(cmdcpy) - 1);
            cmdcpy[sizeof(cmdcpy) - 1] = '\0';
            char *tok = strtok(cmdcpy, " ");
            int j = 0;
            while (tok != NULL && j < 255)
            {
                args[j++] = tok;
                tok = strtok(NULL, " ");
            }
            args[j] = NULL;
            execvp(args[0], args);
            perror("execvp error");
            exit(1);
        }
        else if (pid > 0)
        {
            if (i > 0)
                close(in_fd);
            if (i < num_comandos - 1)
            {
                close(fd[1]);
                in_fd = fd[0];
            }
        }
        else
        {
            perror("fork error");
            return;
        }
    }
    for (i = 0; i < num_comandos; i++) wait(NULL);
}