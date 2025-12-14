#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stdio.h>
#include "Structs.h"

LocalCounter *startSubroutine();
void defineArgVars(LocalCounter *LC,char varName[],Vars *varStorage[]);

#endif