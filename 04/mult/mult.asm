// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Mult.asm

// Multiplies R0 and R1 and stores the result in R2.
// (R0, R1, R2 refer to RAM[0], RAM[1], and RAM[2], respectively.)

// Put your code here.

	@2
	M=0 // sum = 0
	@index
	M=0 // index = 0
(LOOP)
	@index
	D=M
	A=0
	D=D-M
	@FIM
	D;JGE // If (index-RAM0)>=0 GOTO ANS
	@2
	D=M
	A=1
	D=D+M // D = D + RAM1
	@2
	M=D // isere D em sum
	@index
	M=M+1
	@LOOP
	0;JMP // LOOP
(FIM)
	@FIM
	0;JMP // Loop infinito