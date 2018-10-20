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
	void setFileName(string filename); // informa ao codewriter que a tradução de um novo arquivo de VM começou
	void writeArithmetic(string command); // escreve o código assembly que é a tradução da aritmética
	void writePushPop(VMcommand pushOrPop, string segment, int index); // escreve o código assembly que é a tradução do comando push ou pop fornecido
	void close(void); // fecha o arquivo de saida
	void writeInit(void); // escreve código asm predefinido para jump de eq, lt e gt e onde iniciar o código
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
	void pop(string loc, int i); // aparece o topo do valor da pilha para location + endereço do index
	void setOutputFileName(string outname); // define o nome do arquivo de saída, como o diretório

private:
	string filename_;
	string functionName_;
	ofstream outf;
	int labelnum_;
	int start_;
};

#endif
