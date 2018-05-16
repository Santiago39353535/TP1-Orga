#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <ctype.h>
#include "trasponerass.h"

//#define stderr  __stderrp

void validar_cantidad_argumentos(int argc);
char* obtener_output(int argc, char* argv[]);
char* obtener_input(int argc, char* argv[], char* output_filename);
void procesar_archivos(char* output_filename, char* input_filename);
int trasponer(unsigned int filas, unsigned int columnas, long long *entrada, long long *salida);
unsigned long validar_archivo(FILE* fp, unsigned int* filas, unsigned int* columnas);

int main(int argc, char* argv[]) {
  validar_cantidad_argumentos(argc);
  char* output_filename = obtener_output(argc, argv);
  char* input_filename = obtener_input(argc, argv, output_filename);
  procesar_archivos(output_filename, input_filename);

  return 0;
}

void validar_cantidad_argumentos(int argc) {
  if (argc < 2) {
    fprintf( stderr, "Cantidad insuficiente de argumentos.\n");
    exit(1);
  }

  if (argc == 3) {
    fprintf( stderr, "Cantidad incorrecta de argumentos.\n");
    exit(1);
  }

  if (argc > 4) {
    fprintf( stderr, "Cantidad excesiva de argumentos.\n");
    exit(1);
  }
}

char* obtener_output(int argc, char* argv[]) {
  int c;
  const char* short_opt = "Vho:";
  struct option long_opt[] = {
    {"version", no_argument, NULL, 'V'},
    {"help", no_argument, NULL, 'h'},
    {"output", no_argument, NULL, 'o'},
    {NULL, 0, NULL,  0 }
  };

  int option_index = 0;

  while((c = getopt_long(argc, argv, short_opt, long_opt, &option_index)) != -1) {
    switch(c) {
      case 'V':
        if (argc != 2) {
          fprintf( stderr, "Cantidad incorrecta de argumentos.\n");
          exit(1);
        }
        printf("Versión 1.0\n");
        exit(0);
      case 'h':
        if (argc != 2) {
          fprintf( stderr, "Cantidad incorrecta de argumentos.\n");
          exit(1);
        }
        printf("Usage:\n");
        printf("traspuesta -h\n");
        printf("traspuesta -V\n");
        printf("traspuesta [options] filename\n\n");
        printf("Options:\n");
        printf("-h, --help     Prints usage information.\n");
        printf("-V, --version  Prints version information.\n");
        printf("-o, --output   Path to output file.\n\n");
        printf("Examples:\n");
        printf("traspuesta -o - mymatrix\n");
        exit(0);
      case 'o':
        if (argc != 4) {
          fprintf( stderr, "Cantidad incorrecta de argumentos.\n");
          exit(1);
        }
        if (strcmp(optarg, "-") != 0) {
          return optarg;
        }
        break;
    }
  }
  return NULL;
}

char* obtener_input(int argc, char* argv[], char* output_filename) {
  switch(argc) {
    case 2:
      return argv[1];
      break;
    case 4:
      if (strcmp(argv[2], output_filename) == 0) {
        return argv[3];
      } else if (strcmp(argv[3], output_filename) == 0) {
        return argv[2];
      }
      break;
  }
  return "";
}

unsigned long validar_archivo(FILE* fp, unsigned int* filas, unsigned int* columnas) {
  if (fscanf(fp, "%u", filas) == -1 || *filas == -1) {
    fprintf( stderr, "No se ha podido obtener la cantidad de filas.\n");
    exit(1);
  }
  if (fscanf(fp, "%u", columnas) == -1 || *columnas == -1) {
    fprintf( stderr, "No se ha podido obtener la cantidad de columnas.\n");
    exit(1);
  }

  long position = ftell(fp);
  int c;
  while (!feof(fp)) {
    c = fgetc(fp);
    if (!(c == ' ' || c == '\n' || c == EOF || (c >= '0' && c <= '9') || c == '-')) {
      fprintf( stderr, "El archivo no contiene solamente enteros.\n");
      exit(1);
    }
  }

  unsigned int celdas = *filas * *columnas;
  fseek(fp, position, SEEK_SET);
  long long celda;
  unsigned int i;
  for (i = 0; i < celdas; i++) {
    if (fscanf(fp, "%lld", &celda) == -1) {
      fprintf( stderr, "El archivo contiene una cantidad incorrecta de enteros.\n");
      exit(1);
    }
  }

  return position;
}

void procesar_archivos(char* output_filename, char* input_filename) {
  FILE* fp = fopen(input_filename, "r");
  if (fp == 0) {
    fprintf( stderr, "El archivo de entada no existe.\n");
    exit(1);
  }

  unsigned int filas = 0;
  unsigned int columnas = 0;
  unsigned long position = validar_archivo(fp, &filas, &columnas);
  unsigned int celdas = filas * columnas;
  long long* entrada = malloc(celdas * sizeof(long long));
  if (entrada == NULL) {
    fprintf( stderr, "No se ha podido alocar la memoria para la entrada.\n");
    exit(1);
  }
  long long* salida = malloc(celdas * sizeof(long long));
  if (salida == NULL) {
    free(entrada);
    fprintf( stderr, "No se ha podido alocar la memoria para la salida.\n");
    exit(1);
  }

  fseek(fp, position, SEEK_SET);
  unsigned int i;
  for (i = 0; i < celdas; i++) {
    fscanf(fp, "%lld", &entrada[i]);
  }
  fclose(fp);

  trasponer(filas, columnas, entrada, salida);
  //trasponer_ass(filas, columnas, entrada, salida);
  free(entrada);

  if (output_filename == NULL) {
    printf("%u %u\n", columnas, filas);
    int j;
    for (j = 0; j < columnas; j++) {
      int k;
      for (k = 0; k < filas; k++) {
        printf("%lld ", salida[j*filas+k]);
      }
      printf("\n");
    }
  } else {
    FILE* output = fopen(output_filename, "w");
    fprintf(output, "%u %u\n", columnas, filas);
    int j;
    for (j = 0; j < columnas; j++) {
      int k;
      for (k = 0; k < filas; k++) {
        fprintf(output, "%lld ", salida[j*filas+k]);
      }
      fprintf(output, "\n");
    }
    fclose(output);
  }

  free(salida);
}

int trasponer(unsigned int filas, unsigned int columnas, long long *entrada, long long *salida) {
  unsigned int celdas = filas * columnas;
  unsigned int columnasSalida = filas;
  unsigned int i;
  for (i = 0; i < celdas; i++) {
    unsigned int filaEntrada = i / columnas;
    unsigned int columnaEntrada = i % columnas;

    unsigned int filaSalida = columnaEntrada;
    unsigned int columnaSalida = filaEntrada;

    salida[filaSalida * columnasSalida + columnaSalida] = entrada[i];
  }
  return 0;
}
