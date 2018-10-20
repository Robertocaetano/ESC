#ifndef _CODEWRITER_
#define _CODEWRITER_

#include <iostream>
#include <string>
#include <fstream>
#include "Parser.h"
using namespace std;

class CodeWriter
{
public:
	// construtor
	CodeWriter(string filename); // abrir arquivos de saida
	CodeWriter();

	// desconstrutor
	~CodeWriter();

	// metodos
	void setFileName(string filename); // informa ao codewriter que a tradu��o de um novo arquivo de VM come�ou
	void writeArithmetic(string command); // escreve o c�digo assembly que � a tradu��o da aritm�tica
	void writePushPop(VMcommand pushOrPop, string segment, int index); // escreve o c�digo assembly que � a tradu��o do comando push ou pop fornecido
	void close(void); // fecha o arquivo de saida
	void writeInit(void); // escreve c�digo asm predefinido para jump de eq, lt e gt e onde iniciar o c�digo
	void writeLabel(string label); // codigo assembly que afeta o comando label
	void writeGoto(string label); // codigo assembly que afeta o comando goto
	void writeIf(string label); // codigo assembly que afeta o comando if-goto
	void writeCall(string functionName, int numArgs); // codigo assembly que afeta o comando call
	void writeReturn(void); // codigo assembly que afeta o comando return
	void writeFunction(string functionName, int numLocals); // codigo assembly que afeta o comando function

	// metodos de ajuda
	void popD(void); // estoura o topo da pilha do registrador D
	void popGPR(int regNum); // estoura o valor armazenado no registro D para GPR, RAM 13-15
	void decSP(void); // Decrementa o ponteiro da pilha e o registrador de pontos A para o topo do valor da pilha
	void incSP(void); // Incrementa o ponteiro da pilha e o registrador de pontos A no topo da pilha
	void setReturn(int regNum); // Define o local para retornar depois de um salto, armazena em GPR regNum
	void getReturn(int regNum); // Volta para o local armazenado no GPR
	void push(string loc, int i); // envia valor de * (location + index) para o topo da pilha
	void pop(string loc, int i); // aparece o topo do valor da pilha para location + endere�o do index
	void setOutputFileName(string outname); // define o nome do arquivo de sa�da, como o diret�rio

private:
	string filename_;
	string functionName_;
	ofstream outf;
	int labelnum_;
	int start_;
};

#endif
