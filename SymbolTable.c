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