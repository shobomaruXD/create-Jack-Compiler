#ifndef Structs_H
#define Structs_H

#include <stdio.h>
#define N 128
#define HASH_SIZE 101

typedef struct{
    char type[N];
    char value[N];
}Token;

typedef struct{
    FILE *file;
}VMWriter;

typedef struct Symbol{  //一つの変数の情報を保持するnode
    char name[N];
    char type[N];
    char kind[N];     // 種類(static,field,argument,var)
    int index;      //VMセグメントでのインデックス(start from 0),何番目か
    struct Symbol *next; // ハッシュ衝突解決のためのリンク(ポインタ)
}Symbol;

typedef struct SymbolTable{     // シンボルテーブル全体を管理する構造体
    Symbol *classScope[HASH_SIZE];      //ハッシュ配列
    Symbol *subroutineScope[HASH_SIZE]; //ハッシュ配列

    int staticCount;
    int fieldCount;
    int argCount;
    int varCount;
}SymbolTable;

#endif