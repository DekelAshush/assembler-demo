#ifndef SECONDLOOP_H
#define SECONDLOOP_H

#include "globals.h"

#define LABEL_WHITE_SPACE_SKIP while(label[i] == ' ' || label[i] == '\n' || label[i] == '\t'){i++;}


int secondLoop(FILE *assembly_code,char *fileName,int *numError,long *ic,long *dc,dataImg *dataIm,insImg *insIm,Schart *schartHead) ;

int getKeyWord (char* key_word) ;

long checkTheLabelExist(Schart *schartHead ,char* label );

int ifRegister(char* key_word);

int instructionToBinarySecondLoop(insImg *currInsPtr,Schart *head,char *operandOne,char *operandTwo,long *dc,long *adress,int sourceOperand,int targetOperand,int sourceOperandMethod,int targetOperandMethod,int opcode,int numOperands, int lineNumber,int *numErrors,extChart **extHead,extChart **extTail);

#endif