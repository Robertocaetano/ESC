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
		cerr << "Erro: Nenhum arquivo ou diretório fornecido.\n";
	else
	{
		for(int i=1;i<argc;i++)
		{
			string filename = argv[i];
			int dotJack = filename.find(".jack",0);
			if(dotJack == -1) // diretório fornecido
			{
				cout << "Suporte de diretório atualmente não disponível. Apenas forneça todos os arquivos .jack separados por um espaço.\n";
				return(1);
			}
			else
			{
				JackTokenizer JT(filename);
				string outputFile = filename.substr(0,dotJack) + ".vm"; // saída de um arquivo vm para cada arquivo de jack.
				VMWriter VM(outputFile);
				SymbolTable ST;
				CompilationEngine CE(&JT,&VM,&ST);
			}
		}
	}
}
