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
	SymbolTable(); // Cria uma nova tabela de símbolos vazia

	// Methods
	void startSubroutine(void); // Inicia um novo escopo de sub-rotina (redefine a tabela de símbolos da sub-rotina)
	void define(string name, string type, symboltable::Kind kind); // Define um novo identificador de um determinado nome, tipo e tipo e atribui a ele um índice em execução. symboltable :: STATIC e FIELD têm escopo de classe, ARG e VAR têm escopo de sub-rotina
	int varCount(symboltable::Kind kind); //  Retorna o número de variáveis do tipo dado já definido no escopo atual
	symboltable::Kind kindOf(string name); // Retorna o tipo do identificador nomeado no escopo atual. Se o identificador for desconhecido no escopo atual, retornará NONE
	string typeOf(string name); // Retorna o tipo do identificador nomeado no escopo atual
	int indexOf(string name); // Retorna o índice atribuído ao identificador nomeado


private:
	map<string, symboltable::ST> class_scope_; // tabela hash para escopo de classe
	map<string, symboltable::ST> subroutine_scope_; // tabela de hash para o escopo da sub-rotina
	int static_count_; //  mantém um índice em execução de variáveis estáticas para o escopo da classe
	int field_count_; // mantém um índice de variáveis de campo para o escopo da classe
	int arg_count_; //  mantém um índice em execução de variáveis de argumento para o escopo da sub-rotina
	int var_count_; //  mantém um índice em execução de variáveis var para o escopo da sub-rotina
};

#endif
