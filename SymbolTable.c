#include <stdio.h>
#include "SymbolTable.h"
#include "Structs.h"

SymbolTable *createSymbolTable(){
    SymbolTable *table=(SymbolTable *)malloc(sizeof(SymbolTable));    // initing hashtable
    table->staticCount=0;
    table->fieldCount=0;
    return table;
}

void startSubroutine(SymbolTable *table){   // initing subroutine table
    table->argCount=0;
    table->varCount=0;
}

void define(SymbolTable *table,const char *name,const char *type,const char *kind){
    int index=0;
    if(strcmp(kind,"static")==0){
        index=table->staticCount++; //classScopeに登録
    }else if(strcmp(kind,"field")==0){
        index=table->fieldCount++;  //classScopeに登録
    }else if(strcmp(kind,"argument")==0){
        index=table->argCount++;    //subroutineScopeに登録
    }else if(strcmp(kind,"var")==0){
        index=table->varCount++;    //subroutineScopeに登録
    }
}
