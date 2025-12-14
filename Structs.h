#ifndef Structs_H
#define Structs_H

#include <stdio.h>
#define N 128
#define HASH_SIZE 10000

typedef struct{
    char type[N];
    char value[N];
}Token;

typedef struct{
    FILE *file;
}VMWriter;

typedef struct Symbol{
    char name[N];
    char type[N];
    char kind[N];     // 種類(static,field,argument,var)
    int index;
    struct Symbol *next; // ハッシュ衝突のためのリンク
}Symbol;

typedef struct SymbolTable{     // シンボルテーブル全体を管理する構造体
    Symbol *classScope[HASH_SIZE]; 
    Symbol *subroutineScope[HASH_SIZE];

    int staticCount;
    int fieldCount;
    int argCount;
    int varCount;
}SymbolTable;

#endif