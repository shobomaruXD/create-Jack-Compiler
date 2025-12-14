#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stdio.h>
#include "Structs.h"

SymbolTable *createSymbolTable();
void startSubroutine(SymbolTable *table);

int hash(const char *key);
void define(SymbolTable *table,const char *name,const char *type,const char *kind);
Symbol *search(SymbolTable *table,const char *name);

#endif