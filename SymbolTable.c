#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SymbolTable.h"
#include "Structs.h"

SymbolTable *createSymbolTable(){
    SymbolTable *table=(SymbolTable *)calloc(1,sizeof(SymbolTable));    // initing hashtable
    table->staticCount=0;
    table->fieldCount=0;
    return table;
}

void startSubroutine(SymbolTable *table){   // initing subroutine table
    table->argCount=0;
    table->varCount=0;
}

int hash(const char *key){  //文字列をユニークな数字に変換する関数
    unsigned long hashValue=0;
    for(int i=0;key[i]!='\0';i++){
        hashValue=key[i]+(hashValue<<6)+(hashValue<<16)-hashValue;
    }
    return hashValue%HASH_SIZE;    //hashValue%HASH_SIZE=UniqueName,HASH_SIZEに収まるように
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

    Symbol *newSymbol=(Symbol *)malloc(sizeof(Symbol));   //newSymbolに情報をコピー
    strcpy(newSymbol->name,name);
    strcpy(newSymbol->type,type);
    strcpy(newSymbol->kind,kind);
    newSymbol->index=index;
    int indexInTable=hash(name);    //ハッシュ関数で配列のインデックスを決定
    Symbol **targetScope;

    if(strcmp(kind,"static")==0||strcmp(kind,"field")==0){
        targetScope=table->classScope;
    }else{
        targetScope=table->subroutineScope;
    }

    newSymbol->next=targetScope[indexInTable]; // 今の先頭をnextにする
    targetScope[indexInTable]=newSymbol;       // 新しいノードを先頭にする:next=newSymbol
}

Symbol *search(SymbolTable *table,const char *name){    //nameから対応する構造体を取得する関数
    int indexInTable=hash(name);
    while(table->subroutineScope[indexInTable]!=NULL){
        Symbol *tempSymbol=table->subroutineScope[indexInTable];
        if(strcmp(tempSymbol->name,name)==0){
            return tempSymbol;
        }
        tempSymbol=tempSymbol->next;
    }
    while(table->classScope[indexInTable]!=NULL){
        Symbol *tempSymbol=table->classScope[indexInTable];
        if(strcmp(tempSymbol->name,name)){
            return tempSymbol;
        }
        tempSymbol=tempSymbol->next;
    }
    return NULL;
}