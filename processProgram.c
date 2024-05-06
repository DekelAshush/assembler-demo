#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "globals.h"
#include "errors.h"
#include "readFile.h"
#include "macro.h"
#include "secondLoop.h"
#include "utils.h"


int processProgram(char* file) {

    /*declarations*/

    /*data image & instruction Image & symbol table declarations*/
    dataImg dataIm[MAX_INPUT];/*the code can be up to 4096 words*/
    insImg  insIm[MAX_INPUT];/*the data can be up to 4096 words*/
    Schart *schartHead = NULL;
    mcNode *mcHead = NULL;
    /*later we will check length of data Image and instruction Image together wont pass 4096*/
    long ic = IC, dc = DC , point;
    FILE *assembly_code = NULL;
    int errors = CORRECT, none = 0;
    char argFile[LABEL_MAX_LENGTH]; /* Buffer to store the modified filename*/
    strcpy(argFile, file); /* Copy the original filename to the buffer*/

    strcat(argFile, ".as");/*set argumnet file name: "file" to be "file.as"*/

    if (!(assembly_code = fopen(argFile,"r"))){
        print_errors(DONT_EXIST_FILE_ERROR, off, &none, argFile) ;
        print_errors(EQUATE_TO_ZERO, off, off, NULL);
        return FAILED;
    }
    printf("checking '%s' file\n",argFile);

    /*starting now the first pass macro layout... */
    errors= macroLayout(assembly_code,argFile,&none,&mcHead);
    print_errors(GET_NUM_OF_ERRORS, off, &none, NULL) ;

    fclose(assembly_code);/*finished with file.as now we will use file.am*/

    if(errors==FAILED){
        if(mcHead != NULL) {/*free macro table*/
            freeMacroTableNodes(mcHead);
        }
        return FAILED;
    }


    /*starting now the second pass ... */

    point= strchr(argFile,'.') -argFile;/*find the position of '.' in file name*/
    argFile[point]='\0';
    strcat(argFile,".am");   /*instead of "file".as now its "file".am */

    if (!(assembly_code = fopen(argFile,"r"))){/*open the file to read*/
        print_errors(DONT_EXIST_FILE_ERROR, off, &none, argFile) ;
        print_errors(EQUATE_TO_ZERO, off, off, NULL);
        return FAILED;
    }

    errors = readAssemblyFile(assembly_code,mcHead,&none,&ic,&dc,dataIm,insIm,&schartHead);

    if(errors==FAILED){
        if(schartHead != NULL) {/*free symbol table*/
            freeSymbolTableNodes(schartHead);
        }
        fclose(assembly_code);
        printf("the file:'%s' failed on first loop\n\n",argFile);
        return FAILED;
    }
    /*starting now third pass ...*/
    ic = IC;/*reset ic*/
    errors = secondLoop(assembly_code,argFile,&none,&ic,&dc,dataIm,insIm,schartHead);

    if(errors==FAILED){
        printf("the file:'%s' failed on second loop\n\n",argFile);
        return FAILED;
    }


    point= strchr(argFile,'.') -argFile;/*find the position of '.' in file name*/
    argFile[point]='\0';/*remove the .somthing....*/
    strcat(argFile,".as");   /*instead of "file".as now its "file".am */

    printf("the file:'%s' finished process succesfully!\n\n",argFile);
    return SUCCESSED;

}
