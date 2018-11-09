#include "CompilationEngine.h"
#include "JackTokenizer.h"
#include "SymbolTable.h"
#include "VMWriter.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <assert.h>
using namespace std;

namespace CEhelper
{
	string keyWord2String(keyword Key)
	{
		switch(Key)
		{
			case CLASS:
				return "class";
			case METHOD:
				return "method";
			case FUNCTION:
				return "function";
			case CONSTRUCTOR:
				return "constructor";
			case INT:
				return "int";
			case BOOLEAN:
				return "boolean";
			case CHAR:
				return "char";
			case VOID:
				return "void";
			case VAR:
				return "var";
			case STATIC:
				return "static";
			case FIELD:
				return "field";
			case LET:
				return "let";
			case DO:
				return "do";
			case IF:
				return "if";
			case ELSE:
				return "else";
			case WHILE:
				return "while";
			case RETURN:
				return "return";
			case TRUE:
				return "true";
			case FALSE:
				return "false";
			case null:
				return "null";
			case THIS:
				return "this";
		}
	}

	symboltable::Kind keyWord2Kind(keyword Key)
	{
		switch(Key)
		{
			case STATIC:
				return symboltable::STATIC;
			case FIELD:
				return symboltable::FIELD;
			case VAR:
				return symboltable::VAR;
		}
	}

	vm::Segment kind2Segment(symboltable::Kind kind)
	{
		switch(kind)
		{
			case symboltable::STATIC:
				return vm::STATIC;
			case symboltable::FIELD:
				return vm::THIS;
			case symboltable::ARG:
				return vm::ARG;
			case symboltable::VAR:
				return vm::LOCAL;
		}
	}
}

CompilationEngine::CompilationEngine(JackTokenizer * JT, VMWriter * VM, SymbolTable * ST)
{
	JT_ = JT;
	VM_ = VM;
	ST_ = ST;
	numArgs_=0;
	labelNum_=0;
	compileClass();
}

CompilationEngine::~CompilationEngine()
{
	VM_->close();
	cout << "Compilacao completa!" << endl;
}

void CompilationEngine::compileClass(void)
{
	JT_->advance();
	assert(JT_->keyWord() == CLASS);
	JT_->advance();
	assert(JT_->tokenType() == IDENTIFIER);
	className_ = JT_->identifier();
	JT_->advance();
	assert(JT_->symbol() == '{');
	JT_->advance();
	while((JT_->tokenType() == KEYWORD) && (JT_->keyWord() == STATIC || JT_->keyWord() == FIELD || JT_->keyWord() == CONSTRUCTOR || JT_->keyWord() == FUNCTION || JT_->keyWord() == METHOD || JT_->keyWord() == VOID))
	{
		switch(JT_->keyWord())
		{
			case STATIC:
			case FIELD:
				compileClassVarDec();
				break;
			case CONSTRUCTOR:
			case FUNCTION:
			case METHOD:
				compileSubroutine();
				break;
			default:
				break;
		}
	}
	assert(JT_->symbol() == '}');
}

void CompilationEngine::compileClassVarDec(void)
{
	string name, type;
	symboltable::Kind kind;
	// qual o tipo?
	kind = CEhelper::keyWord2Kind(JT_->keyWord());
	JT_->advance();

	if(JT_->tokenType() == KEYWORD)
	{
		type = CEhelper::keyWord2String(JT_->keyWord());
		JT_->advance();
	}
	else if(JT_->tokenType() == IDENTIFIER) // um nome de class
	{
		type = JT_->identifier();
		JT_->advance();
	}
	while(JT_->getCurrentToken() != ";")
	{
		switch(JT_->tokenType())
		{
			case IDENTIFIER: // var name
				name = JT_->identifier();
				ST_->define(name, type, kind); // adicione-o à tabela de símbolos
				break;
			case SYMBOL:
				JT_->updateCurrentToken(); // apenas tire os simbolos
				break;
		}
		if(JT_->getCurrentToken() == "")
			JT_->advance();
	}
	assert(JT_->symbol() == ';');
	JT_->advance();
}

void CompilationEngine::compileSubroutine(void)
{
	ST_->startSubroutine();
	subroutineType_ = JT_->keyWord();
	JT_->advance();
	if(subroutineType_ == METHOD)
		ST_->define("this",className_,symboltable::ARG); // primeiro argumento de um metodo é sempre "this"
	string returnType;
	if(JT_->tokenType() == KEYWORD)
	{
		returnType = CEhelper::keyWord2String(JT_->keyWord());
		JT_->advance();
	}
	else if(JT_->tokenType() == IDENTIFIER)
	{
		returnType = JT_->identifier();
		JT_->advance();
	}
	if(JT_->tokenType() == IDENTIFIER)
	{
		currentFunctionName_ = className_ + "." + JT_->identifier();
		if(JT_->getCurrentToken() == "")
			JT_->advance();
		else if(JT_->symbol() == '(')
		{
			JT_->updateCurrentToken();
			if(JT_->getCurrentToken()=="")
				JT_->advance();
			compileParameterList();
		}
		if(JT_->getCurrentToken() == ")")
		{
			JT_->updateCurrentToken();
		}
		if(JT_->getCurrentToken() == "")
			JT_->advance();
		if(JT_->getCurrentToken() == "{")
			compileSubroutineBody();
	}
	JT_->advance();
}

void CompilationEngine::compileParameterList(void)
{
	string name, type;
	symboltable::Kind kind = symboltable::ARG;
	int SM = 0;
	while(JT_->getCurrentToken() != ")")
	{
		switch(JT_->tokenType())
		{
			case KEYWORD:
				if(SM == 0)
				{
					type = CEhelper::keyWord2String(JT_->keyWord());
					JT_->advance();
					SM=1;
				}
				break;
			case IDENTIFIER:
				if(SM==1)
				{
					name = JT_->identifier();
					ST_->define(name, type, kind);
					SM=0;
				}
				else if(SM==0)
				{
					type = JT_->identifier(); // tipo definido pelo usuario
					JT_->advance();
					SM=1;
				}
				break;
			case SYMBOL:
				JT_->updateCurrentToken();
				SM=0;
				break;
		}
		if(JT_->getCurrentToken() == "")
			JT_->advance();
	}
}

void CompilationEngine::compileSubroutineBody(void)
{
	JT_->updateCurrentToken();
	if(JT_->getCurrentToken() == "")
		JT_->advance();
	while(JT_->keyWord() == VAR)
		compileVarDec();
	int nLocals = ST_->varCount(symboltable::VAR);
	VM_->writeFunction(currentFunctionName_, nLocals);
	if(subroutineType_ == CONSTRUCTOR)
	{
		VM_->writePush(vm::CONST,ST_->varCount(symboltable::FIELD));
		VM_->writeCall("Memory.alloc", 1);
		VM_->writePop(vm::POINTER,0);
	}
	else if(subroutineType_ == METHOD) // define a base deste segmento corretamente, de modo que esta função esteja disponível para todas as instâncias da classe
	{
		VM_->writePush(vm::ARG, 0); // push "this" do objeto chamado
		VM_->writePop(vm::POINTER, 0); // set "this" do method para o objeto de chamada
	}
	if(JT_->keyWord() == LET || JT_->keyWord() == IF || JT_->keyWord() == ELSE || JT_->keyWord() == WHILE || JT_->keyWord() == DO || JT_->keyWord() == RETURN)
		compileStatements();
	assert(JT_->symbol() == '}');
}

void CompilationEngine::compileVarDec(void)
{
	string name, type;
	symboltable::Kind kind;
	// tipo e var
	kind = CEhelper::keyWord2Kind(JT_->keyWord());
	JT_->advance();
	int SM=0;
	while(JT_->getCurrentToken() != ";")
	{
		switch(JT_->tokenType())
		{
			case KEYWORD:
				if(SM==0)
				{
					type = CEhelper::keyWord2String(JT_->keyWord());
					JT_->advance();
					SM=1;
					break;
				}
			case IDENTIFIER:
				if(SM==1)
				{
					name = JT_->identifier();
					ST_->define(name, type, kind);
					break;
				}
				else if(SM==0)
				{
					type = JT_->identifier();
					JT_->advance();
					SM=1;
					break;
				}
			case SYMBOL:
				JT_->updateCurrentToken();
				break;
		}
		if(JT_->getCurrentToken() == "")
			JT_->advance();
	}
	if(JT_->getCurrentToken() == ";")
	{
		JT_->advance();
	}
}

void CompilationEngine::compileStatements(void)
{
	while((JT_->tokenType() == KEYWORD) && (JT_->keyWord() == LET || JT_->keyWord() == IF || JT_->keyWord() == ELSE || JT_->keyWord() == WHILE || JT_->keyWord() == DO || JT_->keyWord() == RETURN))
	{
		switch(JT_->keyWord())
		{
			case LET:
				compileLet();
				break;
			case DO:
				compileDo();
				break;
			case WHILE:
				compileWhile();
				break;
			case RETURN:
				compileReturn();
				break;
			case IF:
				compileIf();
				break;
		}
	}
}

void CompilationEngine::compileExpression(void)
{
	compileTerm();
	while(JT_->getCurrentToken().substr(0,1) != ";" && JT_->getCurrentToken().substr(0,1) != "}" && JT_->getCurrentToken().substr(0,1) != "]" && JT_->getCurrentToken().substr(0,1) != ")" && JT_->getCurrentToken().substr(0,1) != ",")
	{
		switch(JT_->tokenType())
		{
			char op;
			case SYMBOL:
				op = JT_->symbol();
				JT_->updateCurrentToken();
				if(JT_->getCurrentToken() == "")
					JT_->advance();
				compileTerm();
				switch(op)
				{
					case '+':
						VM_->writeArithmetic(vm::ADD);
						break;
					case '-':
						VM_->writeArithmetic(vm::SUB);
						break;
					case '*':
						VM_->writeCall("Math.multiply",2);
						break;
					case '/':
						VM_->writeCall("Math.divide",2);
						break;
					case '&':
						VM_->writeArithmetic(vm::AND);
						break;
					case '|':
						VM_->writeArithmetic(vm::OR);
						break;
					case '<':
						VM_->writeArithmetic(vm::LT);
						break;
					case '>':
						VM_->writeArithmetic(vm::GT);
						break;
					case '=':
						VM_->writeArithmetic(vm::EQ);
						break;
					case '~':
						VM_->writeArithmetic(vm::NOT);
						break;
				}
				break;
			default:
				compileTerm();
				break;
		}
		if(JT_->getCurrentToken() == "")
			JT_->advance();
	}
}

void CompilationEngine::compileTerm(void)
{
	string id, str_const;
	switch(JT_->tokenType())
	{
		case IDENTIFIER:
			id = JT_->identifier();
			if(JT_->getCurrentToken() != "")
			{
				if(JT_->symbol() == '(') // chamada de subrotina
				{
					JT_->updateCurrentToken();
					if(JT_->getCurrentToken() == "")
						JT_->advance();
					VM_->writePush(vm::POINTER,0); // pushing this
					compileExpressionList(); // push argumentos na pilha
					if(JT_->symbol() == ')')
					{
						JT_->updateCurrentToken();
						if(JT_->getCurrentToken() == "")
							JT_->advance();
						if(numArgs_ > 0)
							VM_->writeCall(className_+"."+id, numArgs_+1); // numArgs ajuda a determinar quantos argumentos existem, adicione outro argumento para "this"
						else // o único argumento é "this"
							VM_->writeCall(className_+"."+id, 1);
						numArgs_=0; // redefinir para o proxima chamada
					}
				}
				else if(JT_->symbol() == '.')
				{
					JT_->updateCurrentToken();
					string subroutineName = JT_->identifier();
					if(ST_->kindOf(id)!=symboltable::NONE) // se é um objeto armazenado em symboltable, então vamos chamar seus próprios métodos
					{
						VM_->writePush(CEhelper::kind2Segment(ST_->kindOf(id)),ST_->indexOf(id)); // pushing a própria referência do objeto para a pilha
						if(JT_->symbol() == '(')
						{
							JT_->updateCurrentToken();
						}
						if(JT_->getCurrentToken() == "")
							JT_->advance();
						compileExpressionList();
						if(JT_->symbol() == ')')
						{
							JT_->updateCurrentToken();
							if(JT_->getCurrentToken() == "")
								JT_->advance();
						}
						VM_->writeCall(ST_->typeOf(id)+"."+subroutineName,numArgs_+1); // chamar o método da classe predefinida com sua própria referência colocada na pilha para operar em seus próprios campos
					}
					else // chamando um método de classe incorporado, e Name já se refere ao seu tipo
					{
						if(JT_->symbol() == '(')
						{
							JT_->updateCurrentToken();
						}
						if(JT_->getCurrentToken() == "")
							JT_->advance();
						compileExpressionList(); // coloque os argumentos na pilha
						if(JT_->symbol() == ')')
						{
							JT_->updateCurrentToken();
							if(JT_->getCurrentToken() == "")
								JT_->advance();
						}
						VM_->writeCall(id+"."+subroutineName,numArgs_); //chamada construída no método
					}
					numArgs_=0;
				}
				else if(JT_->symbol() == '[')
				{
					JT_->updateCurrentToken();
					if(JT_->getCurrentToken() == "")
						JT_->advance();
					compileExpression();
					VM_->writePush(CEhelper::kind2Segment(ST_->kindOf(id)),ST_->indexOf(id));
					VM_->writeArithmetic(vm::ADD);
					if(JT_->symbol() == ']')
					{
						JT_->updateCurrentToken();
					}
					VM_->writePop(vm::POINTER,1); // ponteiro de endereço pop em ponteiro 1
					VM_->writePush(vm::THAT,0); // coloque o valor do ponteiro de endereço de volta para a pilha
				}
				else
				{
					VM_->writePush(CEhelper::kind2Segment(ST_->kindOf(id)), ST_->indexOf(id));
				}
			}
			else
			{
				VM_->writePush(CEhelper::kind2Segment(ST_->kindOf(id)), ST_->indexOf(id));
			}
			break;
		case INT_CONST:
			VM_->writePush(vm::CONST, JT_->intVal());
			break;
		case STRING_CONST:
			str_const = JT_->stringVal();
			VM_->writePush(vm::CONST,str_const.length());
			VM_->writeCall("String.new",1);
			for(int i=0;i<str_const.length();i++)
			{
				VM_->writePush(vm::CONST,(int)str_const[i]);
				VM_->writeCall("String.appendChar",2);
			}
			break;
		case KEYWORD: // true, false, null, this atualizará automaticamente o token atual ao chamar keyWord ()
			switch(JT_->keyWord())
			{
				case TRUE:
					VM_->writePush(vm::CONST,0);
					VM_->writeArithmetic(vm::NOT);
					break;
				case FALSE:
				case null:
					VM_->writePush(vm::CONST,0);
					break;
				case THIS:
					VM_->writePush(vm::POINTER,0); // this se refere ao ponteiro 0
					break;
			}
			break;
		case SYMBOL:
			if(JT_->symbol() == '(')
			{
				JT_->updateCurrentToken();
				if(JT_->getCurrentToken() == "")
					JT_->advance();
				compileExpression();
				assert(JT_->symbol() == ')');
				JT_->updateCurrentToken();
			}
			else //unaryOp term
			{
				char uOp = JT_->symbol();
				JT_->updateCurrentToken();
				if(JT_->getCurrentToken() == "")
					JT_->advance();
				compileTerm();
				switch(uOp)
				{
					case '-':
						VM_->writeArithmetic(vm::NEG);
						break;
					case '~':
						VM_->writeArithmetic(vm::NOT);
						break;
					default:
						cerr << "nao e um operador unary" << endl;
						break;
				}
			}
			break;
	}
	if(JT_->getCurrentToken() == "")
		JT_->advance();
}

void CompilationEngine::compileExpressionList(void)
{
	while(JT_->getCurrentToken().substr(0,1) != ")")
	{
		compileExpression();
		numArgs_++;
		if(JT_->getCurrentToken().substr(0,1)==",")
		{
			JT_->updateCurrentToken();
			if(JT_->getCurrentToken() == "")
				JT_->advance();
		}
	}
}

void CompilationEngine::compileDo()
{
	JT_->advance();
	string Name = JT_->identifier(); // subroutineName ou className ou varName
	if(JT_->symbol() == '(') // Chamando o próprio método de classe, coloque isso como primeiro argumento
	{
		JT_->updateCurrentToken();
		if(JT_->getCurrentToken() == "")
			JT_->advance();
		VM_->writePush(vm::POINTER,0); // pushing this
		compileExpressionList();
		if(JT_->symbol() == ')')
		{
			JT_->updateCurrentToken();
			if(JT_->getCurrentToken() == "")
				JT_->advance();
		}
		if(numArgs_ > 0)
			VM_->writeCall(className_+"."+Name,numArgs_+1);
		else // o unico argumento e "this"
			VM_->writeCall(className_+"."+Name,1);
		numArgs_=0;
	}
	else if(JT_->symbol() == '.') // Método Jack, o primeiro argumento forçado deve ser uma referência ao objeto no qual o método deve funcionar
	{
		JT_->updateCurrentToken();
		string subroutineName = JT_->identifier();
		if(ST_->kindOf(Name)!=symboltable::NONE)
		{
			VM_->writePush(CEhelper::kind2Segment(ST_->kindOf(Name)),ST_->indexOf(Name));
			if(JT_->symbol() == '(')
			{
				JT_->updateCurrentToken();
			}
			if(JT_->getCurrentToken() == "")
				JT_->advance();
			compileExpressionList();
			if(JT_->symbol() == ')')
			{
				JT_->updateCurrentToken();
				if(JT_->getCurrentToken() == "")
					JT_->advance();
			}
			VM_->writeCall(ST_->typeOf(Name)+"."+subroutineName,numArgs_+1);
		}
		else // chamando um método de classe incorporado, e Name já se refere ao seu tipo
		{
			if(JT_->symbol() == '(')
			{
				JT_->updateCurrentToken();
			}
			if(JT_->getCurrentToken() == "")
				JT_->advance();
			compileExpressionList();
			if(JT_->symbol() == ')')
			{
				JT_->updateCurrentToken();
				if(JT_->getCurrentToken() == "")
					JT_->advance();
			}
			VM_->writeCall(Name+"."+subroutineName,numArgs_);
		}
		numArgs_=0;
	}
	assert(JT_->symbol() == ';');
	JT_->advance();
	VM_->writePop(vm::TEMP,0);
}

void CompilationEngine::compileLet(void)
{
	JT_->advance();
	string LHSvar = JT_->identifier();
	int isArray = 0;
	if(JT_->getCurrentToken() == "")
		JT_->advance();
	if(JT_->symbol() == '[')
	{
		JT_->updateCurrentToken();
		if(JT_->getCurrentToken() == "")
			JT_->advance();
		compileExpression();
		VM_->writePush(CEhelper::kind2Segment(ST_->kindOf(LHSvar)),ST_->indexOf(LHSvar));
		VM_->writeArithmetic(vm::ADD); // Mantenha o endereço no topo da pilha até que seja necessário, porque se for colocado no ponteiro 1, pode ser substituído e perdido
		if(JT_->symbol() == ']')
		{
			JT_->updateCurrentToken();
			if(JT_->getCurrentToken() == "")
				JT_->advance();
		}
		isArray=1;
	}
	if(JT_->symbol() == '=')
	{
		JT_->updateCurrentToken();
		if(JT_->getCurrentToken() == "")
			JT_->advance();
		compileExpression();
		if(isArray == 1)
			VM_->writePop(vm::TEMP,0); // armazenar resultado em temp 0
	}
	assert(JT_->symbol() == ';');
	if(isArray==0)
	{
		VM_->writePop(CEhelper::kind2Segment(ST_->kindOf(LHSvar)), ST_->indexOf(LHSvar));
		JT_->advance();
	}
	else
	{
		VM_->writePop(vm::POINTER,1); // ponteiro de endereço pop em ponteiro 1
		VM_->writePush(vm::TEMP,0); // empurre o resultado de volta para a pilha
		VM_->writePop(vm::THAT, 0); // Armazene a avaliação do lado direito em THAT 0.
		JT_->advance();
	}
}

void CompilationEngine::compileWhile(void)
{
	stringstream labelTrue, labelFalse;
	labelTrue << "WHILETRUE" << labelNum_;
	labelFalse << "WHILEFALSE" << labelNum_;
	labelNum_++;
	JT_->updateCurrentToken();
	if(JT_->getCurrentToken().substr(0,1) != "(")
		JT_->advance();
	assert(JT_->symbol() == '(');
	JT_->updateCurrentToken();
	if(JT_->getCurrentToken() == "")
		JT_->advance();
	VM_->writeLabel(labelTrue.str()); // while true label execute as instrucoes novamente
	compileExpression();
	VM_->writeArithmetic(vm::NOT);
	VM_->writeIf(labelFalse.str()); // If ~(condition) é true, pule as declarações e vá para labelFalse, else execute as declarações
	assert(JT_->symbol() == ')');
	JT_->updateCurrentToken();
	if(JT_->getCurrentToken() == "")
		JT_->advance();
	assert(JT_->symbol() == '{');
	JT_->updateCurrentToken();
	if(JT_->getCurrentToken() == "")
		JT_->advance();
	compileStatements();
	VM_->writeGoto(labelTrue.str()); // Volte para labelTrue e verifique a condição
	VM_->writeLabel(labelFalse.str()); // Quebra de loop while porque ~ (condição) é true
	assert(JT_->symbol() == '}');
	JT_->updateCurrentToken();
	if(JT_->getCurrentToken() == "")
		JT_->advance();
}

void CompilationEngine::compileReturn(void)
{
	if(JT_->getCurrentToken() == "return")
	{
		JT_->advance();
		compileExpression();
		assert(JT_->symbol() == ';');
		VM_->writeReturn();
		JT_->advance();
	}
	else if(JT_->getCurrentToken() == "return;")
	{
		VM_->writePush(vm::CONST,0);
		VM_->writeReturn();
		JT_->advance();
	}
}

void CompilationEngine::compileIf(void)
{
	stringstream labelElse, labelContinue;
	labelElse << "IFFALSE" << labelNum_;
	labelContinue << "CONTINUE" << labelNum_;
	labelNum_++;
	JT_->updateCurrentToken();
	JT_->updateCurrentToken(); // feito duas vezes porque "if" é de 2 letras
	if(JT_->getCurrentToken() == "")
		JT_->advance();
	assert(JT_->symbol() == '(');
	JT_->updateCurrentToken();
	if(JT_->getCurrentToken() == "")
		JT_->advance();
	compileExpression();
	VM_->writeArithmetic(vm::NOT);
	VM_->writeIf(labelElse.str());
	assert(JT_->symbol() == ')');
	JT_->updateCurrentToken();
	if(JT_->getCurrentToken() == "")
		JT_->advance();
	assert(JT_->symbol() == '{');
	JT_->updateCurrentToken();
	if(JT_->getCurrentToken() == "")
		JT_->advance();
	compileStatements();
	VM_->writeGoto(labelContinue.str());
	assert(JT_->symbol() == '}');
	JT_->updateCurrentToken();
	if(JT_->getCurrentToken() == "")
		JT_->advance();
	VM_->writeLabel(labelElse.str());
	if(JT_->tokenType() == KEYWORD && JT_->keyWord() == ELSE)
	{
		JT_->updateCurrentToken();
		JT_->updateCurrentToken();
		JT_->updateCurrentToken();
		JT_->updateCurrentToken(); // feito 4 vezes porque "else" é de 4 letras
		if(JT_->getCurrentToken() == "")
			JT_->advance();
		assert(JT_->symbol() == '{');
		JT_->updateCurrentToken();
		if(JT_->getCurrentToken() == "")
			JT_->advance();
		compileStatements();
		assert(JT_->symbol() == '}');
		JT_->updateCurrentToken();
		if(JT_->getCurrentToken() == "")
			JT_->advance();
	}
	VM_->writeLabel(labelContinue.str());
}
