# 🔍 Proyecto: Creación de Shell

## 👥 Integrantes del Equipo

| Nombre | GitHub | Matrícula |
|--------|--------|-----------|
| Ignacio Jesús Soto Miranda |  [@Liivne](http://github.com/Liivne)  | 2023447412 | 
| Lucas Daniel Morales Oyanedel | [@Falling-Bridge](https://github.com/Falling-Bridge) | 2023441490 |
| Vicente Ignacio Miranda Gómez | [@VicenteM32](https://github.com/VicenteM32) | 2023412848 |
| Eduardo Alfonso Mora Hernández| [@Eddomora](https://github.com/Eddomora) | 2023403989 |

## Introducción.
En el presente proyecto hacemos uso de nuestros conocimientos aprendidos en Sistemas Operativos para la creación de una shell (intérprete de comandos) básica programada en C para Linux.
Algunas de sus características principales son:
 * Ejecuta comandos simples.
 * Soporta pipes múltiples.
 * Comando exit para salir de la Shell.
 * Manejo de procesos hijos con fork() y execvp().
 * Parseo de comandos con manejo de espacios.

## Estructura del proyecto
```
.
├── Readme.md
├── include
│   ├── Comando_personalizado.h
│   ├── Comando_pipes.h
│   ├── librerias.h
│   └── navegacion.h
├── makefile
├── obj
├── shell
└── src
    ├── Comando_personalizado.c
    ├── Comando_pipes.c
    ├── main.c
    └── navegacion.c
```


## Compilación, ejecución y uso.
#### Compilación (sin Makefile)
Para compilación 
```
gcc -Wall -Wextra -Iinclude src/main.c src/navegacion.c src/Comando_personalizado.c src/Comando_pipes.c -o main -lreadline
```
Para ejecución 
```
/main
```

#### Compilación (Con Makefile)
Solo para compilación
```
make compile
```
Solo para ejecución
```
make run
```
Para eliminar el ejecutable generado y los archivos de tipo .o en la carpeta 'obj'
```
make clean
```
 
#### Manual de Uso.
- Sintaxis Básica: $ [argumento1][argumento2] , algunos ejemplos de uso:
  * $ ls (navegación de archivos)
  * $ ls -la (navegación de archivos)
  * ps aux (proceso del sistema)
  * echo "Hola mundo" (Utilidades)
- Sintaxis con pipes: $ [argumento1] | [argumento2] | ... , algunos ejemplos de uso:
  * $ ls | grep .c
  * $ ps -aux | sort -nr -k 4 | head -20

## Implementación de Funciones.
#### Función main(): Flujo principal
 * Contiene el bucle principal de la shell.
 * Lectura de entrada, captura comandos con fgets.
 * Detecta pipes, usa token con strtok() con | como delimitador
 * Se distingue entre comandos simples y pipelines.
 * Verifica finalización con el comando exit.
#### Función ejecutar_pipes(): Manejo de pipes.
* Por cada comando en el pipeline crea un pipe (excepto el último comando).
* Proceso padre coordina la creación de pipes y espera la finalización de hijos.
* Existe sincronización con el uso de wait() evitando procesos zombie.

