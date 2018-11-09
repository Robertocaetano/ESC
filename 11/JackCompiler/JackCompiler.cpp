#include "JackTokenizer.h"
#include "JackTokenizer.cpp"
#include "CompilationEngine.h"
#include "CompilationEngine.cpp"
#include "SymbolTable.h"
#include "SymbolTable.cpp"
#include "VMWriter.h"
#include "VMWriter.cpp"
#include <iostream>
#include <string>
using namespace std;

int main(int argc, char *argv[])
{
	if(argc == 1)
		cerr << "Erro: Nenhum arquivo ou diret�rio fornecido.\n";
	else
	{
		for(int i=1;i<argc;i++)
		{
			string filename = argv[i];
			int dotJack = filename.find(".jack",0);
			if(dotJack == -1) // diret�rio fornecido
			{
				cout << "Suporte de diret�rio atualmente n�o dispon�vel. Apenas forne�a todos os arquivos .jack separados por um espa�o.\n";
				return(1);
			}
			else
			{
				JackTokenizer JT(filename);
				string outputFile = filename.substr(0,dotJack) + ".vm"; // sa�da de um arquivo vm para cada arquivo de jack.
				VMWriter VM(outputFile);
				SymbolTable ST;
				CompilationEngine CE(&JT,&VM,&ST);
			}
		}
	}
}
