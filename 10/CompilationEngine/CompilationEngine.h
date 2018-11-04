#ifndef _COMPILATIONENGINE_
#define _COMPILATIONENGINE_

#include "JackTokenizer.h"
#include <iostream>
#include <fstream>
using namespace std;

class CompilationEngine
{
public:
	// Constructor
	CompilationEngine(JackTokenizer * JT, string outputFile); // Cria um novo mecanismo de compilação com a entrada e saída fornecidas. A próxima rotina chamada deve ser compileClass ()
	// Destructor
	~CompilationEngine();

	// Methods
	void compileClass(void); // Compila uma class completa
	void compileClassVarDec(void); // Compila uma declaração estática ou uma declaração de campo
	void compileSubroutine(void); // Compila um método, função ou construtor completo
	void compileParameterList(void); // Compila uma lista de parâmetros (possivelmente vazia), não incluindo o "()" fechado
	void compileSubroutineBody(void); // Compila o corpo da sub-rotina
	void compileVarDec(void); // Compila uma declaração var
	void compileStatements(void); // Compila uma sequência de instruções, não incluindo o {} anexo.
	void compileDo(void); // Compila uma declaração do
	void compileLet(void); // Compila uma declaração let
	void compileWhile(void); // compila uma declaração While
	void compileReturn(void); // Compila uma declaração return
	void compileIf(void); // Compila uma declaração if, possível com uma cláusula else posterior
	void compileExpression(void); // compila uma expressao
	void compileTerm(void); // Compila um termo. usa uma unica lookahead
	void compileExpressionList(void); // Compila uma lista de expressões separadas por vírgula (possivelmente vazia)

private:
	JackTokenizer * JT_;
	ofstream fout_;
};

#endif
