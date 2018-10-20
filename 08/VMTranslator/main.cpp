#include <iostream>
#include <string>
#include "Parser.h"
#include "CodeWriter.h"
#include "Parser.cpp"
#include "CodeWriter.cpp"
using namespace std;


int main(int argc, char *argv[])
{
	CodeWriter asm1;
	Parser vm1;
	for(int i=1;i<argc;i++)
	{
		string filename = argv[i];
		int dot = filename.find(".",0);
		if(dot == -1)
		{
			asm1.setOutputFileName(filename); // nome de saida
		}
		else
		{
			filename = filename.substr(0,dot);
			vm1.setFileName(filename);
			asm1.setFileName(filename);
		}
		while (vm1.hasMoreCommands())
		{
			vm1.advance();
			if (vm1.commandType() == C_ARITHMETIC)
			{
				asm1.writeArithmetic(vm1.arg1());
			}
			else if (vm1.commandType() == C_POP)
			{
				asm1.writePushPop(C_POP, vm1.arg1(), vm1.arg2());
			}
			else if (vm1.commandType() == C_PUSH)
			{
				asm1.writePushPop(C_PUSH, vm1.arg1(), vm1.arg2());
			}
			else if (vm1.commandType() == C_LABEL)
			{
				asm1.writeLabel(vm1.arg1());
			}
			else if(vm1.commandType() == C_GOTO)
			{
				asm1.writeGoto(vm1.arg1());
			}
			else if(vm1.commandType() == C_IF)
			{
				asm1.writeIf(vm1.arg1());
			}
			else if(vm1.commandType() == C_FUNCTION)
			{
				asm1.writeFunction(vm1.arg1(), vm1.arg2());
			}
			else if(vm1.commandType() == C_RETURN)
			{
				asm1.writeReturn();
			}
			else if(vm1.commandType() == C_CALL)
			{
				asm1.writeCall(vm1.arg1(), vm1.arg2());
			}
		}
	}
}
