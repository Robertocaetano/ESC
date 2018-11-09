#ifndef _SYMBOLTABLE_
#define _SYMBOLTABLE_

#include <string>
#include <map>
using namespace std;

namespace symboltable
{
	enum Kind { STATIC, FIELD, ARG, VAR, NONE };

	struct ST
	{
		string type;
		Kind kind;
		int index;
	};
}

class SymbolTable
{
public:
	// Constructor
	SymbolTable(); // Cria uma nova tabela de s�mbolos vazia

	// Methods
	void startSubroutine(void); // Inicia um novo escopo de sub-rotina (redefine a tabela de s�mbolos da sub-rotina)
	void define(string name, string type, symboltable::Kind kind); // Define um novo identificador de um determinado nome, tipo e tipo e atribui a ele um �ndice em execu��o. symboltable :: STATIC e FIELD t�m escopo de classe, ARG e VAR t�m escopo de sub-rotina
	int varCount(symboltable::Kind kind); //  Retorna o n�mero de vari�veis do tipo dado j� definido no escopo atual
	symboltable::Kind kindOf(string name); // Retorna o tipo do identificador nomeado no escopo atual. Se o identificador for desconhecido no escopo atual, retornar� NONE
	string typeOf(string name); // Retorna o tipo do identificador nomeado no escopo atual
	int indexOf(string name); // Retorna o �ndice atribu�do ao identificador nomeado


private:
	map<string, symboltable::ST> class_scope_; // tabela hash para escopo de classe
	map<string, symboltable::ST> subroutine_scope_; // tabela de hash para o escopo da sub-rotina
	int static_count_; //  mant�m um �ndice em execu��o de vari�veis est�ticas para o escopo da classe
	int field_count_; // mant�m um �ndice de vari�veis de campo para o escopo da classe
	int arg_count_; //  mant�m um �ndice em execu��o de vari�veis de argumento para o escopo da sub-rotina
	int var_count_; //  mant�m um �ndice em execu��o de vari�veis var para o escopo da sub-rotina
};

#endif
