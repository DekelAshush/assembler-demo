#ifndef READFILE_H
#define READFILE_H
#include "globals.h"

#define PARAMETER_WHITE_SPACE_SKIP while(temp_curr_param[i] == ' ' || temp_curr_param[i] == '\n' || temp_curr_param[i] == '\t'){i++;}


int readAssemblyFile(FILE *assembly_code,mcNode *mcHead,int *numError,long *ic,long *dc,dataImg *dataIm,insImg *insIm,Schart **schartHead);


#endif