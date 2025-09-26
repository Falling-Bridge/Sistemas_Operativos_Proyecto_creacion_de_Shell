CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
LDFLAGS = -lreadline
SRCDIR = src
SOURCES = $(SRCDIR)/Shell.c $(SRCDIR)/navegacion.c $(SRCDIR)/Comando_personalizado.c
TARGET = shell

# Regla principal: compilar
compile: $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES) $(LDFLAGS)

# Regla: correr el programa
run: $(TARGET)
	./$(TARGET)

# Regla: compilar y correr a la vez
all: compile run

# Limpiar archivos compilados
clean:
	rm -f $(TARGET)

.PHONY: compile run all clean