#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stdio.h>
#include "Structs.h"

SymbolTable *createSymbolTable();
void startSubroutine(SymbolTable *table);

#endif