#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "secondLoop.h"
#include "errors.h"
#include "firstLoop.h"
#include "globals.h"
#include "utils.h"
#include "createFile.h"

extChart *addExternalChart(char *label, long adress, int currentNumberLine,int *numError);

int InsImageLineCreateSecondLoop(Schart *head,insImg *currInsPtr, long *adress,long *dc, int opcode, char* operands, int lineNumber,int *numErrors,extChart **extHead,extChart **extTail) ;

int addressToData(dataImg *data_image,Schart *symbolNameExist,long *dc) ;

void updateEntryToSymbolTable(Schart *schartHead,char *label, int currentNumberLine,int *numError) ;

int secondLoop(FILE *assembly_code,char *fileName,int *numError,long *ic,long *dc,dataImg *dataIm,insImg *insIm,Schart *schartHead) {

    extChart *extHead=NULL,*extTail=NULL;/*ill save here all the external file adresses that apears in the code*/
    int currentNumberLine = 0, typeOfSentence,keyword;
    char currLine[LINE_MAX_LENGTH],label[LABEL_MAX_LENGTH], key_word[KEY_WORD_MAX_LENGTH], operands[LINE_MAX_LENGTH];
    char temp_curr_line[LINE_MAX_LENGTH];
    fpos_t p1;


    rewind(assembly_code);/*file.am set to start*/
    fgetpos(assembly_code,&p1);/*;p1 gets the current file location(first letter of first row)*/
	*ic=*ic +PROGRAM_NUMBER; /*how much to add ic to give him his starting address,in this program the number is 100*/

    /*start reading from assembly file*/
     while (fgets(currLine, LINE_MAX_LENGTH, assembly_code) != NULL) {/*checked row length in macroLayout so can go throw all wor without check*/
        resetVars(label,operands,key_word);
        currentNumberLine++;
        /*read the current line and define the type.*/

        typeOfSentence = whatLineAmI((const char *) &currLine);

        /*not an empty or note line*/
		if(typeOfSentence == INSTRUCTION_LINE || typeOfSentence == GUIDANCE_LINE){

			strcpy(temp_curr_line, currLine);
            /*get the data from the current line*/
			getCommands(temp_curr_line, label, key_word, operands, currentNumberLine,numError,SECOND);/*fill the label, the key word and the data of the line.*/

		    if(typeOfSentence == INSTRUCTION_LINE ){
		        keyword=getKeyWord(key_word);
		    	(*ic) += InsImageLineCreateSecondLoop(schartHead,insIm, ic,dc, keyword, operands, currentNumberLine,numError,&extHead,&extTail);
		    }/*end if(typeOfSentence == INSTRUCTION_LINE)*/
			else {/*GUIDANCE_LINE*/
		        keyword=getKeyWord(&key_word[1]);/*send key word without '.' (point)*/
		            if(keyword==ENTRY){
		                updateEntryToSymbolTable(schartHead,operands,currentNumberLine,numError);
		            }
				}/*end of guidance line*/
		    }/*end if(typeOfSentence == INSTRUCTION_LINE || typeOfSentence == GUIDANCE_LINE)*/
    		setbuf(stdout, 0);
		}/*END while(!feof(assembly_code)) loop */

	/*if we found error stop loopone and return false*/
	print_errors(GET_NUM_OF_ERRORS, off, numError, NULL) ;
	if(*numError!=0) {
	    if(schartHead != NULL) {/*free symbol table*/
	        freeSymbolTableNodes(schartHead);
	    }
	    if(extHead != NULL) {/*free external table*/
	        freeExtTableNodes(extHead);
	    }
	    fclose(assembly_code);
		return FAILED;
	}

    createFiles(insIm,dataIm,extHead,fileName,schartHead,ic,dc);

    if(schartHead != NULL) {/*free symbol table*/
        freeSymbolTableNodes(schartHead);
    }

    if(extHead != NULL) {/*free external table*/
        freeExtTableNodes(extHead);
    }

    fclose(assembly_code);


	return SUCCESSED;
}


extChart *addExternalChart(char *label, long adress, int currentNumberLine,int *numError){

    extChart *curr_external_node=NULL;

    /*allocation*/
    curr_external_node =(extChart *)malloc(sizeof(extChart));
    if (curr_external_node == NULL) {
        print_errors(ALLOCATION_ERROR, currentNumberLine, numError, NULL);
        printf("Memory allocation failed for new sChart node\n");
        return NULL;
    }

    curr_external_node->address=adress;
    strcpy(curr_external_node->symbol,label);
    curr_external_node->next=NULL;

    return curr_external_node;
}


void updateEntryToSymbolTable(Schart *schartHead,char *label, int currentNumberLine,int *numError) {

	int check_label,i=0;
    int label_length=0;
    char temp_curr_label[LABEL_MAX_LENGTH];

    LABEL_WHITE_SPACE_SKIP;
    while(label[i] != ' ' && label[i] != '\n' && label[i] != '\t' && label[i] != '\0') {
        temp_curr_label[label_length]= label[i];
        label_length++;
        i++;
    }
    LABEL_WHITE_SPACE_SKIP
    if(label[i]!='\0') {
        print_errors(TOO_MANY_ENTRY_ARGUMENTS,currentNumberLine,numError,&label[0]);
        return;
    }
    temp_curr_label[label_length] ='\0';



	check_label= isLegalLabel(schartHead,temp_curr_label,currentNumberLine,numError,ARR);
	if(check_label==SUCCESSED) {
		while(schartHead != NULL) {
		    if(!strcmp(schartHead -> feature, "external")) {/*cant have define and extern with the same name*/
		        if(!strcmp(schartHead -> symbol, temp_curr_label)) {
		            print_errors(LABEL_ALREADY_EXIST_ERROR,currentNumberLine,numError,schartHead->symbol);
		            return;
		        }
		    }
			if(!strcmp(schartHead -> feature, "code") || !strcmp(schartHead -> feature, "data") ) {
				if(!strcmp(schartHead -> symbol, temp_curr_label)) {
					strcpy(schartHead -> feature,"entry");
				    return;
				}
			}
			schartHead = schartHead -> next;
		}/*end while loop*/
	}else {
		print_errors(LABEL_ERROR_NAME,currentNumberLine,numError,temp_curr_label);
	}

	print_errors(LABEL_DONT_EXIST,currentNumberLine,numError,temp_curr_label);
}


int InsImageLineCreateSecondLoop(Schart *head,insImg *currInsPtr, long *adress,long *dc, int opcode, char* operands, int lineNumber,int *numErrors,extChart **extHead,extChart **extTail) {

	int ic=0;
	instructionBinaryLine(head, currInsPtr, opcode, operands, lineNumber,numErrors, &ic,dc, adress,SECOND,extHead, extTail);
	if(ic==EOF) {
		return 0;/*if there is an error dont count ic*/
	}

	return ic;
}


int instructionToBinarySecondLoop(insImg *currInsPtr,Schart *head,char *operandOne,char *operandTwo,long *dc,long *adress,int sourceOperand,int targetOperand,int sourceOperandMethod,int targetOperandMethod,int opcode,int numOperands, int lineNumber,int *numErrors,extChart **extHead,extChart **extTail) {

    int ic=0;/*can use int cuz it wont pass 5 in the function*/
    Schart *symbolNameExist=NULL;
    unsigned int *p1,temp;
    long currentAdress = *adress+ic;/*the address start with the program number */
    int arrayLength=(int)*adress+ic-PROGRAM_NUMBER ;/*i want array to start from 0 i can use int cuz address wont pass 4096 which is input_max_length*/
    extChart *pExt=NULL;
    /**********Instruction line code*************/
            currInsPtr[arrayLength].address=currentAdress;
            currInsPtr[arrayLength].machineCode.A_R_E=0;
            currInsPtr[arrayLength].machineCode.NONE=0;
            currInsPtr[arrayLength].machineCode.opcode=opcode-1;  /*MOV=1 ... */
            currInsPtr[arrayLength].machineCode.addresSourceOp=sourceOperandMethod;/*if TWO_OPERANDS than gets the value else gets 0,if ONE_OPERAND gets value 0*/
            currInsPtr[arrayLength].machineCode.addresTargetOp=targetOperandMethod;/*ZERO_OPERANDS both gets zero*/

            ic=ic+NEXT_ADRESS;
            currentAdress=currentAdress+NEXT_ADRESS;
            arrayLength++;


    if(numOperands==TWO_OPERANDS){
        if(sourceOperandMethod != DIRECT_REGISTER || targetOperandMethod!=DIRECT_REGISTER){
            switch(sourceOperandMethod){
                case IMMIDIATE:
                    currInsPtr[arrayLength].address=currentAdress;
                    currInsPtr[arrayLength].machineCode.A_R_E=0;
                    p1 = (unsigned int *)(&currInsPtr[arrayLength].machineCode) ;
                    *(p1) = sourceOperand<< 2;/*set 12 bits to be sourceOperand value*/

                    ic=ic+NEXT_ADRESS;
                    currentAdress=currentAdress+NEXT_ADRESS;
                    arrayLength++;
                    break;

                case DIRECT:
                    currInsPtr[arrayLength].address=currentAdress;
                    p1 = (unsigned int *)(&currInsPtr[arrayLength].machineCode) ;
                    symbolNameExist=searchSymbolName(head,operandOne);
                    if(symbolNameExist==NULL) {
                        print_errors(LABEL_DONT_EXIST,lineNumber,numErrors,operandOne);
                        return EOF;
                    }if(!strcmp(symbolNameExist->feature,"external")) {
                        pExt = addExternalChart(symbolNameExist->symbol,currentAdress,lineNumber,numErrors);
                        if(pExt!=NULL) {
                            if(*extHead==NULL) {
                                *extHead = pExt;
                                *extTail =pExt;
                            }else {
                                (*extTail)->next = pExt;
                                *extTail = pExt;
                            }
                        }/*adding the adres and the external label to the chart*/
                            *(p1)=symbolNameExist->address<<2;/*move 2 bit and put the adress*/
                            currInsPtr[arrayLength].machineCode.A_R_E=1;/*external label*/

                    }else if(opcode!=LEA) {
                        /*all other opcode with 2 operands mov, cmp, add, sub.... need to go to data segment*/
                        if(strcmp(symbolNameExist->feature,"data") && strcmp(symbolNameExist->feature,"string")  && strcmp(symbolNameExist->feature,"entry")) {
                            print_errors(NOT_EXIST_ADRESS_IN_DATA,symbolNameExist->address,numErrors,NULL);
                            return EOF;
                        }
                        *(p1)=symbolNameExist->address<<2;/*move 2 bit and put the adress*/
                        currInsPtr[arrayLength].machineCode.A_R_E=2;/*external label*/
                    }else{
                        *(p1)=symbolNameExist->address<<2;/*move 2 bit and put the adress*/
                        currInsPtr[arrayLength].machineCode.A_R_E=2;/*label in file Code */
                    }

                    ic=ic+NEXT_ADRESS;
                    currentAdress=currentAdress+NEXT_ADRESS;
                    arrayLength++;
                    break;

                case FIXED_INDEX:
                    currInsPtr[arrayLength].address=currentAdress;
                    p1 = (unsigned int *)(&currInsPtr[arrayLength].machineCode) ;
                    symbolNameExist=searchSymbolName(head,operandOne);/*searching name of array in symbolchart*/
                    if(symbolNameExist==NULL) {
                        print_errors(LABEL_DONT_EXIST,lineNumber,numErrors,operandOne);
                        return EOF;
                    }/*if we found the label*/
                    if(!strcmp(symbolNameExist->feature,"external")) {
                        pExt = addExternalChart(symbolNameExist->symbol,currentAdress,lineNumber,numErrors);
                        if(pExt!=NULL) {
                            if(*extHead==NULL) {
                                *extHead = pExt;
                                *extTail =pExt;
                            }else {
                                (*extTail)->next = pExt;
                                *extTail = pExt;
                            }
                        }
                        *(p1)=0;
                        currInsPtr[arrayLength].machineCode.A_R_E=1;/*external label*/
                        /*external has adress 0 in our program*/
                    }else if(strcmp(symbolNameExist->feature,"data") && strcmp(symbolNameExist->feature,"string")  && strcmp(symbolNameExist->feature,"entry")){
                        print_errors(NOT_EXIST_ADRESS_IN_DATA,symbolNameExist->address,numErrors,NULL);
                        return EOF;/*array have to be in data segment*/
                    }else {
                        *(p1)=symbolNameExist->address<<2;
                        currInsPtr[arrayLength].machineCode.A_R_E=2;/*label in file Code*/
                    }

                    ic=ic+NEXT_ADRESS;
                    currentAdress=currentAdress+NEXT_ADRESS;
                    arrayLength++;
                    /*********************finished label now value inside the label(array)****************************/
                    currInsPtr[arrayLength].address=currentAdress;
                    currInsPtr[arrayLength].machineCode.A_R_E=0;
                    p1 = (unsigned int *)(&currInsPtr[arrayLength].machineCode);/*p1 gets pointer to thestart of the machinecode addres*/
                    *(p1) = sourceOperand <<2;/*set 12 bits to be sourceOperand value*/

                    ic=ic+NEXT_ADRESS;
                    currentAdress=currentAdress+NEXT_ADRESS;
                    arrayLength++;
                    break;

                case DIRECT_REGISTER:
                    currInsPtr[arrayLength].address=currentAdress;
                currInsPtr[arrayLength].machineCode.A_R_E=0;
                p1 = (unsigned int *)(&currInsPtr[arrayLength].machineCode) ;
                *(p1) = sourceOperand << 5;/*set sourceOperand in bits 5-7 */

                ic=ic+NEXT_ADRESS;
                currentAdress=currentAdress+NEXT_ADRESS;
                arrayLength++;
                break;
            }
            switch (targetOperandMethod){
                case IMMIDIATE:
                    currInsPtr[arrayLength].address=currentAdress;
                    currInsPtr[arrayLength].machineCode.A_R_E=0;
                    p1 = (unsigned int *)(&currInsPtr[arrayLength].machineCode) ;
                    *(p1) = targetOperand << 2;/*set 12 bits to be sourceOperand value*/

                    ic=ic+NEXT_ADRESS;
                    currentAdress=currentAdress+NEXT_ADRESS;
                    arrayLength++;
                    break;

                case DIRECT:
                    currInsPtr[arrayLength].address=currentAdress;
                    p1 = (unsigned int *)(&currInsPtr[arrayLength].machineCode) ;
                    symbolNameExist=searchSymbolName(head,operandTwo);
                    if(symbolNameExist==NULL) {
                        print_errors(LABEL_DONT_EXIST,lineNumber,numErrors,operandTwo);
                        return EOF;
                    }if(!strcmp(symbolNameExist->feature,"external")) {
                        pExt = addExternalChart(symbolNameExist->symbol,currentAdress,lineNumber,numErrors);
                        if(pExt!=NULL) {
                            if(*extHead==NULL) {
                                *extHead = pExt;
                                *extTail =pExt;
                            }else {
                                (*extTail)->next = pExt;
                                *extTail = pExt;
                            }
                        }
                        *(p1)=symbolNameExist->address<<2;/*move 2 bit and put the adress*/
                        currInsPtr[arrayLength].machineCode.A_R_E=1;/*external label*/
                    }else if(opcode!=LEA){/*all other opcode with 2 operands mov, cmp, add, sub.... need to go to data segment*/
                        if(strcmp(symbolNameExist->feature,"data") && strcmp(symbolNameExist->feature,"string")  && strcmp(symbolNameExist->feature,"entry")) {
                            print_errors(NOT_EXIST_ADRESS_IN_DATA,symbolNameExist->address,numErrors,NULL);
                            return EOF;
                        }
                        *(p1)=symbolNameExist->address<<2;/*move 2 bit and put the adress*/
                        currInsPtr[arrayLength].machineCode.A_R_E=2;/*external label*/
                    }else{
                        *(p1)=symbolNameExist->address<<2;/*move 2 bit and put the adress*/
                        currInsPtr[arrayLength].machineCode.A_R_E=2;/*label in file Code */
                    }

                    ic=ic+NEXT_ADRESS;
                    currentAdress=currentAdress+NEXT_ADRESS;
                    arrayLength++;
                    break;

                case FIXED_INDEX:
                    currInsPtr[arrayLength].address=currentAdress;
                    p1 = (unsigned int *)(&currInsPtr[arrayLength].machineCode) ;
                    symbolNameExist=searchSymbolName(head,operandTwo);/*searching name of array in symbolchart*/
                    if(symbolNameExist==NULL) {
                        print_errors(LABEL_DONT_EXIST,lineNumber,numErrors,operandTwo);
                        return EOF;
                    }/*if we found the label*/
                    if(!strcmp(symbolNameExist->feature,"external")) {
                        pExt = addExternalChart(symbolNameExist->symbol,currentAdress,lineNumber,numErrors);
                        if(pExt!=NULL) {
                            if(*extHead==NULL) {
                                *extHead = pExt;
                                *extTail =pExt;
                            }else {
                                (*extTail)->next = pExt;
                                *extTail = pExt;
                            }
                        }
                        *(p1)=0;
                        currInsPtr[arrayLength].machineCode.A_R_E=1;/*external label*/
                        /*external has adress 0 in our program*/
                    }else if(strcmp(symbolNameExist->feature,"data") && strcmp(symbolNameExist->feature,"string")  && strcmp(symbolNameExist->feature,"entry")){
                        print_errors(NOT_EXIST_ADRESS_IN_DATA,symbolNameExist->address,numErrors,NULL);
                        return EOF;/*array have to be in data segment*/
                    }else {
                        *(p1)=symbolNameExist->address<<2;
                        currInsPtr[arrayLength].machineCode.A_R_E=2;/*label in file Code*/
                    }

                    ic=ic+NEXT_ADRESS;
                    currentAdress=currentAdress+NEXT_ADRESS;
                    arrayLength++;
                    /*********************finished label now value inside the labe(array)****************************/
                    currInsPtr[arrayLength].address=currentAdress;
                    currInsPtr[arrayLength].machineCode.A_R_E=0;
                    p1 = (unsigned int *)(&currInsPtr[arrayLength].machineCode);/*p1 gets pointer to thestart of the machinecode addres*/
                    *(p1) = targetOperand <<2;/*set 12 bits to be sourceOperand value*/

                    ic=ic+NEXT_ADRESS;
                    currentAdress=currentAdress+NEXT_ADRESS;
                    arrayLength++;
                    break;

                case DIRECT_REGISTER:
                    currInsPtr[arrayLength].address=currentAdress;
                    currInsPtr[arrayLength].machineCode.A_R_E=0;
                    p1 = (unsigned int *)(&currInsPtr[arrayLength].machineCode) ;
                    *(p1) = targetOperand<< 2 ;/*set targetOperand in bits 2-4*/

                    ic=ic+NEXT_ADRESS;
                    currentAdress=currentAdress+NEXT_ADRESS;
                    arrayLength++;
                    break;

            }
        }else {/*both operands are registers*/
            currInsPtr[arrayLength].address=currentAdress;
            currInsPtr[arrayLength].machineCode.A_R_E=0;
            p1 = (unsigned int *)(&currInsPtr[arrayLength].machineCode) ;
            *(p1) = sourceOperand << 5;/*set sourceOperand in bits  5-7 */
            temp = targetOperand << 2;
            *(p1) =  *(p1)+temp ;/*set targetOperand in bits 2-4 */

            ic=ic+NEXT_ADRESS;
            currentAdress=currentAdress+NEXT_ADRESS;
            arrayLength++;

        }
    }else if(numOperands==ONE_OPERAND){
        /*if TWO_OPERANDS*/
        switch(targetOperandMethod) {
                case IMMIDIATE:/*PRN CASE*/
                    currInsPtr[arrayLength].address=currentAdress;
                    currInsPtr[arrayLength].machineCode.A_R_E=0;
                    p1 = (unsigned int *)(&currInsPtr[arrayLength].machineCode) ;
                    *(p1) = targetOperand <<2;/*set 12 bits to be sourceOperand value*/

                    ic=ic+NEXT_ADRESS;
                    currentAdress=currentAdress+NEXT_ADRESS;
                    arrayLength++;
                    break;

                    case DIRECT:
                        currInsPtr[arrayLength].address=currentAdress;
                        p1 = (unsigned int *)(&currInsPtr[arrayLength].machineCode) ;
                        symbolNameExist=searchSymbolName(head,operandOne);
                        if(symbolNameExist==NULL) {
                            print_errors(LABEL_DONT_EXIST,lineNumber,numErrors,operandOne);
                            return EOF;
                        }
                        if(!strcmp(symbolNameExist->feature,"external")) {
                            pExt = addExternalChart(symbolNameExist->symbol,currentAdress,lineNumber,numErrors);
                            if(pExt!=NULL) {
                                if(*extHead==NULL) {
                                    *extHead = pExt;
                                    *extTail =pExt;
                                }else {
                                    (*extTail)->next = pExt;
                                    *extTail = pExt;
                                }
                            }
                            *(p1)=symbolNameExist->address<<2;/*move 2 bit and put the adress*/
                            currInsPtr[arrayLength].machineCode.A_R_E=1;/*external label*/
                        }else if(opcode!=JMP && opcode!=BNE && opcode!=JSR ){/*all other opcode with 1 operands "not, clr, inc, dec, red, prn" need to go to data segment*/
                            if(strcmp(symbolNameExist->feature,"data") && strcmp(symbolNameExist->feature,"string")  && strcmp(symbolNameExist->feature,"entry")) {
                                print_errors(NOT_EXIST_ADRESS_IN_DATA,symbolNameExist->address,numErrors,NULL);
                                return EOF;
                            }/*jmp,bne,jsr only need the adress*/
                            *(p1)=symbolNameExist->address<<2;/*move 2 bit and put the adress*/
                            currInsPtr[arrayLength].machineCode.A_R_E=2;/*external label*/
                        }else{
                            *(p1)=symbolNameExist->address<<2;/*move 2 bit and put the adress*/
                            currInsPtr[arrayLength].machineCode.A_R_E=2;/*label in file Code */
                        }

                        ic=ic+NEXT_ADRESS;
                        currentAdress=currentAdress+NEXT_ADRESS;
                        arrayLength++;
                        break;

                case FIXED_INDEX:
                    currInsPtr[arrayLength].address=currentAdress;
                    p1 = (unsigned int *)(&currInsPtr[arrayLength].machineCode) ;
                    symbolNameExist=searchSymbolName(head,operandOne);/*searching name of array in symbolchart*/
                    if(symbolNameExist==NULL) {
                        print_errors(LABEL_DONT_EXIST,lineNumber,numErrors,operandOne);
                        return EOF;
                    }/*if we found the label*/
                    if(!strcmp(symbolNameExist->feature,"external")) {
                        pExt = addExternalChart(symbolNameExist->symbol,currentAdress,lineNumber,numErrors);
                        if(pExt!=NULL) {
                            if(*extHead==NULL) {
                                *extHead = pExt;
                                *extTail =pExt;
                            }else {
                                (*extTail)->next = pExt;
                                *extTail = pExt;
                            }
                        }
                        *(p1)=0;
                        currInsPtr[arrayLength].machineCode.A_R_E=1;/*external label*/
                        /*external has adress 0 in our program*/
                    }else if(strcmp(symbolNameExist->feature,"data") && strcmp(symbolNameExist->feature,"string")  && strcmp(symbolNameExist->feature,"entry")){
                        print_errors(NOT_EXIST_ADRESS_IN_DATA,symbolNameExist->address,numErrors,NULL);
                        return EOF;/*array have to be in data segment*/
                    }else {
                        *(p1)=symbolNameExist->address<<2;
                        currInsPtr[arrayLength].machineCode.A_R_E=2;/*label in file Code*/
                    }

                    ic=ic+NEXT_ADRESS;
                    currentAdress=currentAdress+NEXT_ADRESS;
                    arrayLength++;

                    /*********************finished label now value inside the labe(array)****************************/
                    currInsPtr[arrayLength].address=currentAdress;
                    currInsPtr[arrayLength].machineCode.A_R_E=0;
                    p1 = (unsigned int *)(&currInsPtr[arrayLength].machineCode);/*p1 gets pointer to thestart of the machinecode addres*/
                    *(p1) = targetOperand <<2;/*set 12 bits to be sourceOperand value*/

                    ic=ic+NEXT_ADRESS;
                    currentAdress=currentAdress+NEXT_ADRESS;
                    arrayLength++;
                    break;

                case DIRECT_REGISTER:
                    currInsPtr[arrayLength].address=currentAdress;
                    currInsPtr[arrayLength].machineCode.A_R_E=0;
                    p1 = (unsigned int *)(&currInsPtr[arrayLength].machineCode) ;
                    *(p1) = targetOperand << 2;/*set targetOperandMethod in bits 2-4*/

                    ic=ic+NEXT_ADRESS;
                    currentAdress=currentAdress+NEXT_ADRESS;
                    arrayLength++;
                    break;
            }
    }

    return ic;
}




