# assembler-demo
Final project in a laboratory course in systems programming

--Demo-- https://youtu.be/1Ym07igbhTA

Final project in a laboratory course in systems programming This project aims to build a compiler for a 14-instruction assembly language, for an imaginary 14-bit CPU and 14 bits of memory size.
There is a macro layout that identifies when the system calls the macro. The output takes the 14 bits and transforms them into code in 7 bits.

For bits in binary 00, it converts to '*',

For bits in binary 01, it converts to '#',

For bits in binary 10, it converts to '%', 

For bits in binary 11, it converts to '!'.

Changing some settings allows you to use this compiler for a real computer as needed.
#USE on Linux
  1. install the system compiled by script:
  2. Bring some example files from the "inputs outputs" directory
  3. run: ./assembler 'exampleName.as' # You can run more than one assembly file
  4. The output code files will appear in the main directory, with the amount of instruction and data size at the top of the page.
