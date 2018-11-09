#ifndef _JACKTOKENIZER_
#define _JACKTOKENIZER_

#include <fstream>
#include <string>
using namespace std;

// Token Types
enum tokentype { KEYWORD, SYMBOL, IDENTIFIER, INT_CONST, STRING_CONST };

// keyWord
enum keyword { CLASS, METHOD, FUNCTION, CONSTRUCTOR, INT, BOOLEAN, CHAR, VOID, VAR, STATIC, FIELD, LET, DO, IF, ELSE, WHILE, RETURN, TRUE, FALSE, null, THIS };

class JackTokenizer
{
public:
	// Constructor
	JackTokenizer(string inputFile); // Abre o arquivo de entrada e prepara-se para tokenizá-lo
	// Destructor
	~JackTokenizer();

	// Methods
	bool hasMoreTokens(void); // Temos mais tokens na entrada?
	void advance(void); // Obtém o próximo token da entrada e torna o token atual. Este método só é chamado se hasMoreTokens for true. Inicialmente, não há token atual.
	tokentype tokenType(void); //  Retorna o tipo do token atual
	keyword keyWord(void); // Retorna a palavra-chave que é o token atual. Deve ser chamado apenas quando tokenType for KEYWORD
	char symbol(void); // Retorna o caractere que é o token atual. Deve ser chamado apenas quando tokenType for SYMBOL
	string identifier(void); //  Retorna o identificador que é o token atual. Deve ser chamado apenas quando tokenType for IDENTIFICADOR
	int intVal(void); // Retorna o valor inteiro do token atual. Deve ser chamado apenas quando tokenType for INT_CONST
	string stringVal(void); // Retorna o valor da string do token atual, sem as aspas duplas. Deve ser chamado apenas quando tokenType for STRING_CONST
	string getCurrentToken(void); // Retorna o currentToken
	void updateCurrentToken(void); // Solta o primeiro caractere de currentToken_

private:
	ifstream fin_;
	string currentToken_;

};

#endif
