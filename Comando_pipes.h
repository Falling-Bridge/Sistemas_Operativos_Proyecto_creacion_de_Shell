#ifndef COMANDOPIPE_H
#define COMANDOPIPE_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>

void ejecutar_pipes(char *comandos[], int num_comandos);
#endif
