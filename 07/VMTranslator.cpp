#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <iterator>

using namespace std;

// == funções de auxilio ==

string apaga(string &str, char del)
{
  size_t primeiro = str.find_first_not_of(del);
  if (primeiro == string::npos)
    return "";
  size_t ultimo = str.find_last_not_of(del);
  return str.substr(primeiro, (ultimo-primeiro+1));
}

template<typename saida>
void divide(const std::string &s, char delim, saida resultado) {
  std::stringstream ss;
  ss.str(s);
  std::string item;
  while (std::getline(ss, item, delim)) {
    *(resultado++) = item;
  }
}

std::vector<std::string> divide(const std::string &s, char delim) {
  std::vector<std::string> elems;
  divide(s, delim, std::back_inserter(elems));
  return elems;
}

// == VM TRADUTOR ==

int lbl = 0;

// push segm index
// setup = @index
//         D=A
//         @loc
//         [A=M]
//         A=A+D
//         D=M
// code: setup
//       @0
//       A=M
//       M=D
//       @0
//       M=M+1
string manipula_push(string segm, string index) {
  string setup        = "@"+index+"\nD=A\n";
  string heap         = "A=M\n";
  string setup_desloca = "A=A+D\nD=M\n";
  string code         = "@0\nA=M\nM=D\n@0\nM=M+1\n";

  if (segm == "constant") {
    ; // nao faz nada
  }
  else if (segm == "static") {
    setup += "@16\n";
    setup += setup_desloca;
  }
  else if (segm == "temp") {
    setup += "@5\n";
    setup += setup_desloca;
  }
  else if (segm == "local") {
    setup += "@LCL\n";
    setup += heap + setup_desloca;
  }
  else if (segm == "argument") {
    setup += "@ARG\n";
    setup += heap + setup_desloca;
  }
  else if (segm == "this") {
    setup += "@THIS\n";
    setup += heap + setup_desloca;
  }
  else if (segm == "that") {
    setup += "@THAT\n";
    setup += heap + setup_desloca;
  }
  else if (segm == "pointer") {
    setup = ((index=="0") ? "@THIS\n" : "@THAT\n");
    setup += "D=M\n";
  }

  return setup + code;
}

// @0
// M=M-1
// A=M
// D=M
// @13
// M=D
//
// @index
// D=A
// @loc
// [A=M]
// D=A+D
// @14
// M=D
// @13
// D=M
// @14
// A=M
// M=D
string manipula_pop(string segm, string index) {
  string code1 = "@0\nM=M-1\nA=M\nD=M\n@13\nM=D\n@"+index+"\nD=A\n@{}\n";
  string heap = "A=M\n";
  string code2 = "D=A+D\n@14\nM=D\n@13\nD=M\n@14\nA=M\nM=D\n";
  string loc;
  string code;

  if (segm == "static") {
    code = code1 + code2;
    loc = "16";
  }
  else if (segm == "temp") {
    code = code1 + code2;
    loc = "5";
  }
  else if (segm == "local") {
    code = code1 + heap + code2;
    loc = "LCL";
  }
  else if (segm == "argument") {
    code = code1 + heap + code2;
    loc = "ARG";
  }
  else if (segm == "this") {
    code = code1 + heap + code2;
    loc = "THIS";
  }
  else if (segm == "that") {
    code = code1 + heap + code2;
    loc = "THAT";
  }
  else if (segm == "pointer") {
    index = ((index=="0") ? "THIS" : "THAT");
    return "@0\nM=M-1\nA=M\nD=M\n@"+index+"\nM=D\n";
  }

  return code.replace(code.find("{}"), 2, loc);
}

// comandos aritmeticos: add, sub, neg, eq, gt, lt, and, or, not
// @0
// M=M-1
// A=M
// D=M
// @0
// M=M-1
// A=M
// }
// [comp]
// { operadores relacionais
// D=M-D - comp
// @LABELX
// D;jump
// D=0
// @LABELY
// 0;JMP
// (LABELX)
// D=-1 - true
// (LABELY)
// @0
// A=M
// M=D
// }
// @0
// M=M+1
string manipula_aritmetica(string op) {
  string code_unario = "@0\nM=M-1\nA=M\n";
  string code = code_unario + "D=M\n@0\nM=M-1\nA=M\n";
  string comp = "";
  stringstream ss; ss << lbl;
  stringstream ss2; ss2 << lbl+1;
  string slbl = ss.str();
  string slbl2 = ss2.str();
  string code_relacionado =
    "D=M-D\n@LABEL"+slbl+"\nD;[]\nD=0\n@LABEL"+slbl2+"\n0;JMP\n(LABEL"+slbl+")\n"
    "D=-1\n(LABEL"+slbl2+")\n@0\nA=M\nM=D\n";
  string relacionado = "";
  string code_end = "@0\nM=M+1\n";

  if (op == "add") {
    comp = "M=M+D\n";
  }
  else if (op == "sub") {
    comp = "M=M-D\n";
  }
  else if (op == "neg") {
    comp = "M=-M\n";
    code = code_unario;
  }
  else if (op == "eq") {
    relacionado = code_relacionado;
    relacionado.replace(relacionado.find("[]"), 2, "JEQ");
    lbl += 2;
  }
  else if (op == "gt") {
    relacionado = code_relacionado;
    relacionado.replace(relacionado.find("[]"), 2, "JGT");
    lbl += 2;
  }
  else if (op == "lt") {
    relacionado = code_relacionado;
    relacionado.replace(relacionado.find("[]"), 2, "JLT");
    lbl += 2;
  }
  else if (op == "and") {
    comp = "M=M&D\n";
  }
  else if (op == "or") {
    comp = "M=M|D\n";
  }
  else if (op == "not") {
    comp = "M=!M\n";
    code = code_unario;
  }

  code += comp + relacionado + code_end;
  return code;
}

string traduzir(string linha) {
  size_t pos_tira;
  vector<string> simbolo;
  string resultado;

  // tira todos comentarios
  pos_tira = linha.find("//");
  if (pos_tira != string::npos) {
    linha = linha.substr(0, pos_tira);
  }

  // apagar linha
  // linha = apaga_espacobranco(linha);
  linha = apaga(linha, ' ');
  linha = apaga(linha, '\n');
  linha = apaga(linha, '\r');
  if (linha.empty()) return "";

  simbolo = divide(linha, ' ');

  if (simbolo[0] == "push") {
    resultado = manipula_push(simbolo[1], simbolo[2]);
  }
  else if (simbolo[0] == "pop") {
    resultado = manipula_pop(simbolo[1], simbolo[2]);
  }
  // comandos aritmeticos: add, sub, neg, eq, gt, lt, and, or, not
  else if (simbolo[0] == "add") {
    resultado = manipula_aritmetica("add");
  }
  else if (simbolo[0] == "sub") {
    resultado = manipula_aritmetica("sub");
  }
  else if (simbolo[0] == "neg") {
    resultado = manipula_aritmetica("neg");
  }
  else if (simbolo[0] == "eq") {
    resultado = manipula_aritmetica("eq");
  }
  else if (simbolo[0] == "gt") {
    resultado = manipula_aritmetica("gt");
  }
  else if (simbolo[0] == "lt") {
    resultado = manipula_aritmetica("lt");
  }
  else if (simbolo[0] == "and") {
    resultado = manipula_aritmetica("and");
  }
  else if (simbolo[0] == "or") {
    resultado = manipula_aritmetica("or");
  }
  else if (simbolo[0] == "not") {
    resultado = manipula_aritmetica("not");
  }

  return resultado;
}

int main(int argc, char** args) {
  string fname_entrada;
  string fname_saida;
  string linha;
  string code;

  // iteração do arquivo .vm para .asm
  for (int i = 1; i < argc; i++) {
    fname_entrada = args[i];
    fname_saida = fname_entrada.substr(0, fname_entrada.find(".")) + ".asm";
    lbl = 0;

    ifstream is(fname_entrada.data());
    ofstream os(fname_saida.data());

    while(getline(is, linha)) {

      code = traduzir(linha);
      if (code != "")
        os << code << endl;
    }
  }

  return 0;
}
