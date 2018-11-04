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
	CompilationEngine(JackTokenizer * JT, string outputFile); // Cria um novo mecanismo de compila��o com a entrada e sa�da fornecidas. A pr�xima rotina chamada deve ser compileClass ()
	// Destructor
	~CompilationEngine();

	// Methods
	void compileClass(void); // Compila uma class completa
	void compileClassVarDec(void); // Compila uma declara��o est�tica ou uma declara��o de campo
	void compileSubroutine(void); // Compila um m�todo, fun��o ou construtor completo
	void compileParameterList(void); // Compila uma lista de par�metros (possivelmente vazia), n�o incluindo o "()" fechado
	void compileSubroutineBody(void); // Compila o corpo da sub-rotina
	void compileVarDec(void); // Compila uma declara��o var
	void compileStatements(void); // Compila uma sequ�ncia de instru��es, n�o incluindo o {} anexo.
	void compileDo(void); // Compila uma declara��o do
	void compileLet(void); // Compila uma declara��o let
	void compileWhile(void); // compila uma declara��o While
	void compileReturn(void); // Compila uma declara��o return
	void compileIf(void); // Compila uma declara��o if, poss�vel com uma cl�usula else posterior
	void compileExpression(void); // compila uma expressao
	void compileTerm(void); // Compila um termo. usa uma unica lookahead
	void compileExpressionList(void); // Compila uma lista de express�es separadas por v�rgula (possivelmente vazia)

private:
	JackTokenizer * JT_;
	ofstream fout_;
};

#endif
