CC = gcc # GCC Compiler
CFLAGS = -ansi -Wall -pedantic # Flags
GLOBALS_FILES = globals.h 
ALL_OBJ =  errors.o readFile.o firstLoop.o secondLoop.o utils.o macro.o createFile.o processProgram.o

assembler:$(ALL_OBJ) $(GLOBALS_FILES)
	$(CC) $(CFLAGS) -g assembler.c  $(ALL_OBJ) -o assembler

errors.o: errors.c errors.h $(GLOBALS_FILES)
	$(CC) -c $(CFLAGS) errors.c  -o errors.o

readFile.o: readFile.c readFile.h $(GLOBALS_FILES)
	$(CC) -c $(CFLAGS) readFile.c  -o readFile.o

firstLoop.o: firstLoop.c firstLoop.h $(GLOBALS_FILES)
	$(CC) -c $(CFLAGS) firstLoop.c  -o firstLoop.o

secondLoop.o: secondLoop.c secondLoop.h $(GLOBALS_FILES)
	$(CC) -c $(CFLAGS) secondLoop.c  -o secondLoop.o

utils.o: utils.c utils.h $(GLOBALS_FILES)
	$(CC) -c $(CFLAGS) utils.c  -o utils.o

macro.o: macro.c macro.h $(GLOBALS_FILES)
	$(CC) -c $(CFLAGS) macro.c  -o macro.o

createFile.o: createFile.c createFile.h $(GLOBALS_FILES)
	$(CC) -c $(CFLAGS) createFile.c  -o createFile.o

processProgram.o: processProgram.c processProgram.h $(GLOBALS_FILES)
	$(CC) -c $(CFLAGS) processProgram.c  -o processProgram.o


