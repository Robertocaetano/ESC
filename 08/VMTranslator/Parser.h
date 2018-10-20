#ifndef _PARSER_
#define _PARSER_

#include <iostream>
#include <fstream>
#include <string>
using namespace std;

// VM commands
enum VMcommand { C_ARITHMETIC, C_PUSH, C_POP, C_LABEL, C_GOTO, C_IF, C_FUNCTION, C_RETURN, C_CALL };

class Parser
{
public:
	// Construtor
	Parser();
	Parser(string filename);

	// fechamento
	~Parser();

	// Metodos
	bool hasMoreCommands(void); // existem mais comandos na entrada
	void advance(void); // lê o próximo comando da entrada, só deve ser chamado se hasMoreCommands () for true
	VMcommand commandType(void); // retorna o tipo do comando atual da VM
	string arg1(void); // retorna o primeiro argumento do comando atual
	int arg2(void); // retorna o segundo argumento do comando atual
	void setFileName(string filename);

private: // atributos
	string filename_;
	string currentCommand_;
	ifstream inf;
};

#endif
