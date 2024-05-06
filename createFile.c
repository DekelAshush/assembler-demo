#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "errors.h"
#include "globals.h"

void binaryCode(char *binary,char *code);

void intToBinary(unsigned int n, int num_bits, char *binary) ;

void createFiles(insImg *insIm,dataImg *dataIm,extChart *extHead,char *fileName,Schart *schartHead, long *ic,long *dc){
    FILE *file=NULL;
    long point;
    unsigned int *p1;
    char curr_object_name[MAX_FILE_NAME], binary_repr[MACHINE_CODE_LENGTH+1],code[MACHINE_CODE_LENGTH/2+1];/* need 1 more for '\0' */
    int error=0,none=0,i=0,flag_open_ent=off;
    /*create the current file name with the correct suffix*/
    strcpy(curr_object_name, fileName);
    point= strchr(curr_object_name,'.') -curr_object_name;/*find the position of '.' in file name*/

    if(*ic-PROGRAM_NUMBER>0){/* if theres is code*/
        curr_object_name[point]='\0';
        strcat(curr_object_name,".ob");    /*instead of "file".as now its "file".ob  */

        if (!(file = fopen(curr_object_name,"w"))){/*open the file to write*/
            print_errors(DONT_EXIST_FILE_ERROR, off, &none, curr_object_name) ;
            print_errors(EQUATE_TO_ZERO, off, off, NULL);
            error=FAILED;
        }
        if(error!=FAILED) {/*file open seccesfully*/
            fprintf(file, "ic:%ld dc:%ld\n", (*ic-PROGRAM_NUMBER),*dc);/*print in file number of instruction row and number of data row*/
            while(i<*ic-PROGRAM_NUMBER) {
                fprintf(file, "%04ld:  ", insIm[i].address);/*print in file the adress of the code*/
                p1 = (unsigned int *)(&insIm[i].machineCode);
                intToBinary(*p1,MACHINE_CODE_LENGTH,binary_repr);/*transfer from unsigned int into binary represantion in char*/
                binaryCode(binary_repr,code);/*transform the binary char into code*/
                fprintf(file, "%s\n", code);/*print the code in the file*/
                i++;
            }
            i=0;
            if(*dc>0){/* if theres is data*/
                while(i<*dc) {
                    fprintf(file, "%04ld:  ",dataIm[i].address);
                    p1 = (unsigned int *)(&dataIm[i].machineCode);
                    intToBinary(*p1,MACHINE_CODE_LENGTH,binary_repr);
                    binaryCode(binary_repr,code);
                    fprintf(file, "%s\n", code);
                    i++;
                }
            }
        }
        fclose(file);/*close file.ob*/
    }

    if(extHead!=NULL) {/*if theres extern that used in the code than print it,if it not used in the code there wont be file.ext*/
        curr_object_name[point]='\0';
        strcat(curr_object_name,".ext");    /*instead of "file".as now its "file".ext  */

        if (!(file = fopen(curr_object_name,"w"))){/*open the file to write*/
            print_errors(DONT_EXIST_FILE_ERROR, off, &none, curr_object_name) ;
            print_errors(EQUATE_TO_ZERO, off, off, NULL);
            error=FAILED;
        }
        if(error!=FAILED) {
            /*file open seccesfully*/
            while(extHead!=NULL) {
                fprintf(file, "%s     %04ld\n", extHead->symbol,extHead->address);/*print in file the adress of the code*/
                extHead=extHead->next;
            }
        }
        fclose(file);/*close file.ext*/
    }

    if(schartHead!=NULL) {
        curr_object_name[point]='\0';
        strcat(curr_object_name,".ent");    /*instead of "file".as now its "file".ext  */
        while(schartHead!=NULL) {
            if(!strcmp(schartHead->feature,"entry") && flag_open_ent==off){/*create a file only if there are entry in th code*/
                /*open file only if found entry*/
                if (!(file = fopen(curr_object_name,"w"))){/*open the file to write*/
                    print_errors(DONT_EXIST_FILE_ERROR, off, &none, curr_object_name) ;
                    print_errors(EQUATE_TO_ZERO, off, off, NULL);
                    error=FAILED;
                }else {/*file open seccesfully*/
                    flag_open_ent=on;
                }
            }
            if(error!=FAILED && flag_open_ent==on && !strcmp(schartHead->feature,"entry")) {
                fprintf(file, "%s     %04ld\n", schartHead->symbol,schartHead->address);/*print in file the adress of the code*/
            }

            schartHead=schartHead->next;
        }
        if(flag_open_ent==on) {
            fclose(file);/*close file.ent*/

        }
    }

}

void intToBinary(unsigned int n, int num_bits, char *binary) {

    int i;
        for (i = num_bits - 1; i >= 0; i--) {
            binary[num_bits - 1 - i] = ((n >> i) & 1) ? '1' : '0';
        }
    binary[num_bits] = '\0'; /*Null-terminate the string*/
}



void binaryCode(char *binary,char *code) {

    char codeRep[3];/*2 char for 2 bit code representaion and one more for'\0' */
    unsigned int num;/*will save here the int value of the char codeRep*/
    int i,j=MACHINE_CODE_LENGTH/2-1,currentBit=MACHINE_CODE_LENGTH-1;/*position 14 in currentBitis '\0',j position 7 is '\0'*/

    while(currentBit+1>0){/*code machine is 7 char half of 14*/
        for (i =1; i >=0; i--) {
           codeRep[i]=binary[currentBit];
           currentBit--;
        }
        codeRep[2]='\0';
        num =  strtol(codeRep, NULL, 2);/*strtol takes three arguments: the string to convert, a pointer to the end of the conversion (which we set to NULL because we're not interested in it), and the base of the number system (which is 2 for binary).*/
        switch(num){
            case(0):/*00*/
                code[j]='*';
                j--;
                break;

            case(1):/*01*/
                code[j]='#';
                j--;
                break;

            case(2):/*10*/
                code[j]='%';
                j--;
                break;

            case(3):/*11*/
                code[j]='!';
                j--;
                break;
        }
        codeRep[0]='\0';
    }

    code[MACHINE_CODE_LENGTH/2]='\0';
}

