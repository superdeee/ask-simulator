# ask-simulator

 Assembly language editor/parser/processor desgined in C++ Qt framework as a school project. It can simulate the behaviour of some of the most popular assembly instructions in a 16bit based enviorment. Consists of a syntax highlighter and code parser for error detection and cool appearance.
 
 <img src="https://user-images.githubusercontent.com/54100395/134165935-de925b46-4456-45ef-8a57-5cdd3221ed27.png" width="800">
 
 

## Overview

### Processor

 Based on a QScriptEngine, basic data structures and plenty of if statements it is able to simulate operation of a simple 16 bit processor:
 * 4 x 16bit registers with high/low differentiation
 * 255 x 16bit memory space (8 bit addressed)
 * 16 bit stack 
 * 19 POWERFUL instructions, including jump instruction
 * I/O devices simulation
   * system clock and date support
   * keyboard input
   * std in/out terminal
 * 5 system interrupts handling

Instructions available are: 
```
MOV LD LDI ST STI ADD ADDI SUB SUBI INC DEC SHL SHR JMP RET PUSH POP END INT
```

Their behaviour is defined in <i>.is</i> file as a tuple: ```keyword|num of args|args types|command```:
```
MOV,2,17,$a=$b
LD,2,26,$a=M$b
LDI,2,18,$a=$b
ST,2,161,M$a=$b
STI,2,162,M$a=$b
ADD,2,17,$a=$a+$b
ADDI,2,18,$a=$a+$b
SUB,2,17,$a=$a-$b
SUBI,2,18,$a=$a-$b
INC,1,16,$a=$a+1
DEC,1,16,$a=$a-1
SHL,1,16,$a=$a*2
SHR,1,16,$a=Math.floor($a/2)
JMP,1,64,#sip<#ip;#ip=$a
RET,0,0,#sip>#ip
PUSH,1,16,#rip<$a
POP,1,16,#rip>$a
END,0,0,end
INT,1,32,!$a
```

### Editor

There has been label and user defined values handling defined, so as to enable the use of jump to label operations and easier memory usage. Syntax highlighting detects operation keywords, arguments differentiation(register/constant/variable), define statements, labels and code comments.

### Parser

Application can detect 6 types of errors - it validates operation keyword, number of arguments, type of arguments, operation syntax labels naming and user-defined variable errors. Register names are case-sensitive, constant values can be described in one of three numeral systems with different prefixes (binary - '0b', decimal - '0d', hex - '0h'). 
 
<img src="https://user-images.githubusercontent.com/54100395/134165592-c24a37b0-d608-4782-a7c1-65caa8853e6c.png" width="800">
