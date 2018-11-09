#ifndef _COMPILATIONENGINE_
#define _COMPILATIONENGINE_

#include "JackTokenizer.h"
#include "SymbolTable.h"
#include "VMWriter.h"
#include <string>
#include <iostream>
//#include <fstream>
using namespace std;

class CompilationEngine
{
public:
	// Constructor
	CompilationEngine(JackTokenizer * JT, VMWriter * VM, SymbolTable * ST); // Cria um novo mecanismo de compila��o com a entrada e sa�da fornecidas. A pr�xima rotina chamada deve ser compileClass ()
	// Destructor
	~CompilationEngine();

	// Methods
	void compileClass(void); // Compila uma classe completa
	void compileClassVarDec(void); // Compila uma declaracao estatica ou uma declaracao de campo
	void compileSubroutine(void); // Compila um metodo, funcao ou construtor completo
	void compileParameterList(void); // Compila uma lista de parametro (possivelmente vazia), nao incluindo o "()" fechado
	void compileSubroutineBody(void); // Compila o corpo da subrotina
	void compileVarDec(void); // Compila uma declaracao var
	void compileStatements(void); // Compila uma sequ�ncia de instru��es, n�o incluindo o {} anexo.
	void compileDo(void); // Compila uma instrucao do
	void compileLet(void); // Compila uma instrucao let
	void compileWhile(void); // Compila uma instrucao while
	void compileReturn(void); // Compila uma instrucao return
	void compileIf(void); //  Compila uma instru��o if, poss�velmente com uma cl�usula else posterior
	void compileExpression(void); // Compila uma expressao
	void compileTerm(void); // Compila um termo.  Usa uma �nica lookahead
	void compileExpressionList(void); // Compila uma lista (possivelmente vazia) de express�es separadas por v�rgula

private:
	JackTokenizer * JT_;
	SymbolTable * ST_;
	VMWriter * VM_;
	string currentFunctionName_;
	keyword subroutineType_;
	string className_;
	int numArgs_;
	int labelNum_;
};

#endif
