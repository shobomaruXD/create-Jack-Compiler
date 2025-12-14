#ifndef Structs_H
#define Structs_H

#include <stdio.h>
#define N 128

typedef struct{
    char type[N];
    char value[N];
}Token;

typedef struct{
    FILE *file;
}VMWriter;

typedef struct{
    int *arg;
    int *field;
    int *Static;
    int *var;
}LocalCounter;

typedef struct{
    char *name;
    char *kind;
}Vars;

#endif