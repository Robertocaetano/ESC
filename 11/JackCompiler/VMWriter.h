#ifndef _VMWRITER_
#define _VMWRITER_

#include <string>
#include <fstream>
using namespace std;

namespace vmwriter
{
	enum Segment { CONST, ARG, LOCAL, STATIC, THIS, THAT, POINTER, TEMP };
	enum Command { ADD, SUB, NEG, EQ, GT, LT, AND, OR, NOT };

	string segment2String(Segment seg)
	{
		switch(seg)
		{
			case CONST:
				return "constant";
			case ARG:
				return "argument";
			case LOCAL:
				return "local";
			case STATIC:
				return "static";
			case THIS:
				return "this";
			case THAT:
				return "that";
			case POINTER:
				return "pointer";
			case TEMP:
				return "temp";
		}
	}

	string command2String(Command com)
	{
		switch(com)
		{
			case ADD:
				return "add";
			case SUB:
				return "sub";
			case NEG:
				return "neg";
			case EQ:
				return "eq";
			case GT:
				return "gt";
			case LT:
				return "lt";
			case AND:
				return "and";
			case OR:
				return "or";
			case NOT:
				return "not";
		}
	}
};
namespace vm=vmwriter;


class VMWriter
{
public:
	// Constructor
	VMWriter(string fileName); // Cria um novo arquivo e o prepara para gravação.

	// Methods
	void writePush(vm::Segment segment, int index); //  Escreve um comando push da VM
	void writePop(vm::Segment segment, int index); // Escreve um comando pop VM
	void writeArithmetic(vm::Command command); //  Escreve um comando aritmético de VM
	void writeLabel(string label); //  Escreve um comando de rótulo de VM
	void writeGoto(string label); // Escreve um comando goto da VM
	void writeIf(string label); //  Escreve um comando if-goto da VM
	void writeCall(string name, int nArgs); // Escreve um comando de chamada de VM
	void writeFunction(string name, int nLocals); // Escreve um comando de função da VM
	void writeReturn(void); // Escreve um comando de retorno da VM
	void close(void); // Fecha o arquivo de saída

private:
	ofstream fout_;
};

#endif
