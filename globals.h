
#ifndef GLOBALS_H
#define GLOBALS_H
#include <stdio.h>
#define IC 0
#define DC 0
#define NEXT_ADRESS 1
#define LINE_MAX_LENGTH 82/* 80 max char and 1 more for '\n' and another one for '\0' when using fgets =82*/
#define MAX_ARRTIBUTE_LENGTH 15 
#define KEY_WORD_MAX_LENGTH 10  
#define LABEL_MAX_LENGTH 31  
#define MACHINE_CODE_LENGTH 14
#define MAX_FILE_NAME 20
#define MAX_REGISTER 13
#define MIN_REGISTER 0  
#define MAX_INPUT 4096 
#define PROGRAM_NUMBER 100
#define MAX_NUM_12_BIT 2047
#define MIN_NUM_12_BIT -2048
#define MAX_NUM_14_BIT 8191
#define MIN_NUM_14_BIT -8192
#define DATA_MAX_LENGTH 72 /*.string ""= 10 chars, 82-10=72 chars  */
#define WRONG 10000



enum flags { 
	off = 0, on = 1
};

 enum booleans {
    FALSE = 0, TRUE = 1
};

enum file_succeeded {  
	SUCCESSED = 0, FAILED = 1    
};


enum operands {
	TWO_OPERANDS=10,
	ONE_OPERAND,
	ZERO_OPERANDS
};




typedef enum opcodes{
    MOV=1,
    CMP, 
    ADD,
    SUB,
    NOT,
    CLR,
    LEA, 
    INC, 
    DEC, 
    JMP, 
    BNE, 
    RED,
    PRN, 
    JSR, 
    RTS, 
    HLT
}opcode;

typedef enum AddressingMethod {
	IMMIDIATE,
	DIRECT,
	FIXED_INDEX,
	DIRECT_REGISTER,
	NOT_EXIST,
	BAD
}AddressingMethod;

 enum AddressingMethodError {
	FIRST,
	SECOND,
	THIRD,
	FORTH,
	FIFTH,
	SIXTH,
 	SEVENTH
};


typedef enum registers{
        R0=1,
        R1,
        R2,
        R3,
        R4,
        R5,
        R6,
        R7
}reg;

typedef enum guidenceWord{
       MACRO=1,
       LABEL,
       DATA,
       STRING,
       ENTRY,
       EXTERN,
       DEFINE,
	   ARR
}gd;

typedef struct BinaryMachineCode{
	unsigned int A_R_E: 2;
	unsigned int addresTargetOp: 2;
	unsigned int addresSourceOp: 2;
	unsigned int opcode: 4;
	unsigned int NONE: 4;
}Bmc;

typedef struct InstructionImage{
	long address;
	char sourceCode[LINE_MAX_LENGTH];
	Bmc machineCode;
}insImg;

typedef struct dataImage{
    long address;
    char sourceCode[LINE_MAX_LENGTH];
    Bmc machineCode;
}dataImg;

typedef struct symbolChart{  
	long address;
	char symbol[LABEL_MAX_LENGTH]; 
    char feature[MAX_ARRTIBUTE_LENGTH]; 
    struct symbolChart *next;
}Schart;

typedef struct externalChart{
	long address;
	char symbol[LABEL_MAX_LENGTH];
	struct externalChart *next;
}extChart;


typedef struct macroNode{
    fpos_t start_macro;
    fpos_t end_macro;
    char macroName[LABEL_MAX_LENGTH];
	int oneTime;
	struct macroNode *next;
}mcNode;

#endif
