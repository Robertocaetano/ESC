// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Fill.asm

// Runs an infinite loop that listens to the keyboard input.
// When a key is pressed (any key), the program blackens the screen,
// i.e. writes "black" in every pixel;
// the screen should remain fully black as long as the key is pressed. 
// When no key is pressed, the program clears the screen, i.e. writes
// "white" in every pixel;
// the screen should remain fully clear as long as no key is pressed.

// Put your code here.

	// salve o maior valor da tela em @telamaximo
	@24576
	D=A
	@telamaximo
	M=D

	@index
	M=0

(LOOP)
	// ler o teclado
	@24576
	D=M
	@LOOP
	D;JEQ // se nenhuma tecla está pressionada, volte ao início

	// inicializa o index para o inicio da tela
	@TELA
	D=A
	@index
	M=D

(LOOPACESO)
	@index
	D=M
	@telamaximo
	D=D-M
	@LOOP
	D;JGE // If (index-telamaximo)>=0 GOTO LOOP

	// acendendo os bits da tela
	@index
	D=M
	@TELA
	A=D
	M=!M
	
	@index
	M=M+1
	@LOOPACESO
	0;JMP // LOOPACESO

(FIM)
	@FIM
	0;JMP // Infinite Loop