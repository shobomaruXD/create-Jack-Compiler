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

int hash(const char *key){  //文字列をHASH_SIZEに収まるようにする関数
    unsigned long hash_value=0;
    for(int i=0;key[i]!="\n";i++){
        hash_value=key[i]+(hash_value<<6)+(hash_value<<16)-hash_value;
    }
    return hash_value%HASH_SIZE;    //hash配列に収まるように
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

    Symbol *newSymbol=(SymbolTable *)malloc(sizeof(SymbolTable));   //newSymbolに情報をコピー
    int indexInArray=hash(name);    //ハッシュ関数で配列のインデックスを決定

    Symbol **target_table=(strcmp(kind,"static")==0||strcmp(kind,"field")==0)
    ? table->classScope 
    : table->subroutineScope;   // cant understand

    newSymbol->next=target_table[indexInArray];
    target_table[indexInArray]=newSymbol;   //chaining
}

Symbol *search(SymbolTable *table,const char *name){    //nameから対応する構造体を取得する関数
    if(strcmp(table->subroutineScope,name)){
        return name;
    }else if(strcmp(table->classScope,name)){
        return name;
    }else{
        return NULL;
    }
}