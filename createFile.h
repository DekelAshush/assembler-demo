#ifndef CREATEFILE_H
#define CREATEFILE_H
#include "globals.h"


void createFiles(insImg *insIm,dataImg *dataIm,extChart *extHead,char *fileName,Schart *schartHead, long *ic,long *dc);
/*
 this function takes all the data that need to be put into file and if it is nesecery it will create a new file
 an put the data in those files
 */

#endif