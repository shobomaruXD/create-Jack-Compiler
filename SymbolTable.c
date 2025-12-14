#include <stdio.h>
#include "SymbolTable.h"
#include "Structs.h"

LocalCounter *startSubroutine(){
    LocalCounter *LC;
    LC->arg=0; LC->field=0; LC->Static=0; LC->var=0;
    return LC;
}

void defineArgVars(LocalCounter *LC,char varName[],Vars *varStorage[]){
    varStorage[*(LC->arg)]->name=varName;
    varStorage[*(LC->arg)]->kind="argument";
    LC->arg++;
}

void defineFeildVars(LocalCounter *LC,char varName[],Vars *varStorage[]){
    varStorage[*(LC->field)]->name=varName;
    varStorage[*(LC->field)]->kind="this";
    LC->field++;
}