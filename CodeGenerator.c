#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Structs.h"
#include "CodeGenerator.h"

VMWriter *createVMWriter(const char *filename){
    VMWriter *writer=malloc(sizeof(VMWriter));
    writer->file=fopen(filename,"w");
    if(!writer->file){
        perror("VMファイルを作成できません");
        exit(1);
    }
    return writer;
}

void closeVMWriter(VMWriter *writer){
    fclose(writer->file);
    free(writer);
}

void writePush(VMWriter *writer,const char *segment,int index){
    fprintf(writer->file,"push %s %d\n",segment,index);
}

void writePop(VMWriter *writer,const char *segment,int index){
    fprintf(writer->file,"pop %s %d\n",segment,index);
}

void writeArithmetic(VMWriter *writer,const char *command){
    fprintf(writer->file,"%s\n",command);
}

void writeLabel(VMWriter *writer,const char *label){
    fprintf(writer->file,"label %s\n",label);
}

void writeGoto(VMWriter *writer,const char *label){
    fprintf(writer->file,"goto %s\n",label);
}

void writeIf(VMWriter *writer,const char *label){
    fprintf(writer->file,"if-goto %s\n",label);
}

void writeCall(VMWriter *writer,const char *name,int nArgs){
    fprintf(writer->file,"call %s %d\n",name,nArgs);
}

void writeFunction(VMWriter *writer, const char *name, int nLocals){
    fprintf(writer->file,"function %s %d\n",name,nLocals);
}

void writeReturn(VMWriter *writer){
    fprintf(writer->file,"return\n");
}
