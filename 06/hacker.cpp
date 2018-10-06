/**
 * Hacker
 *
 * Para compilar utilize:
 * g++ hacker.cpp -std=c++11 -Wall -o hacker
 */

#include <cstdint>
#include <map>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <bitset>

namespace hacker {

	class tabelasimbolo {
	public:
		void add_label(const std::string &symbol, uint16_t endereco)
		{
			m_tabelasimbolo[symbol] = endereco;
		}

		void add_var(const std::string &symbol)
		{
			m_tabelasimbolo[symbol] = m_var_endereco++;
		}

		bool conteudo(const std::string &symbol)
		{
			return m_tabelasimbolo.find(symbol) != m_tabelasimbolo.end();
		}

		uint16_t endereco(const std::string &symbol)
		{
			return m_tabelasimbolo[symbol];
		}

	private:
		std::map<std::string, uint16_t> m_tabelasimbolo = {
			{"0",      0x0000}, // solucao alternativa do stoi que nao reconhece o 0
			{"SP",     0x0000},
			{"LCL",    0x0001},
			{"ARG",    0x0002},
			{"THIS",   0x0003},
			{"THAT",   0x0004},
			{"R0",     0x0000},
			{"R1",     0x0001},
			{"R2",     0x0002},
			{"R3",     0x0003},
			{"R4",     0x0004},
			{"R5",     0x0005},
			{"R6",     0x0006},
			{"R7",     0x0007},
			{"R8",     0x0008},
			{"R9",     0x0009},
			{"R10",    0x000A},
			{"R11",    0x000B},
			{"R12",    0x000C},
			{"R13",    0x000D},
			{"R14",    0x000E},
			{"R15",    0x000F},
			{"SCREEN", 0x4000},
			{"KBD",    0x6000},
		};
		uint16_t m_var_endereco = 0x0010;
	};

	class parser {
	public:
		enum class tipo_comando {
			nenhum,
			a,
			c,
			l,
		};

		parser(const std::string &file)
			: m_file(file, std::ifstream::in)
		{
		}

		~parser()
		{
			if (m_file.is_open())
				m_file.close();
		}

		void reset()
		{
			m_tipo_comando = tipo_comando::nenhum;
			m_line_num = 0;
			m_file.clear();
			m_file.seekg(0);
		}

		bool mais_comandos() const
		{
			return m_file.good();
		}

		void avancar()
		{
			std::string line;
			m_tipo_comando = tipo_comando::nenhum;
			do {
				std::getline(m_file >> std::ws, line);

				// tira todos os espaços
				line.erase(std::remove_if(line.begin(),
				                          line.end(),
				                          [](char x){return std::isspace(x);}),
				           line.end());

				if (line.empty())
					continue;

				// tira todos os comentários da string
				std::string::size_type comment_pos = line.find("//");

				// verifica se a linha tem comentários
				if (comment_pos != 0) {

					// remove os comentários depois do comando
					if (comment_pos != std::string::npos)
						line.erase(comment_pos);

					if (line.find("@") == 0)
						m_tipo_comando = tipo_comando::a;
					else if (line.find("(") == 0 &&
					         line.rfind(")") == line.size() - 1)
						m_tipo_comando = tipo_comando::l;
					else
						m_tipo_comando = tipo_comando::c;
				}
			} while (m_file.good() && m_tipo_comando == tipo_comando::nenhum);

			if (m_tipo_comando != tipo_comando::nenhum) {
				if (m_tipo_comando != tipo_comando::l)
					m_line_num++;
				m_command = line;
			} else
				m_command = "";
		}

		uint16_t line() const
		{
			return m_line_num;
		}

		tipo_comando command() const
		{
			return m_tipo_comando;
		}

		std::string symbol() const
		{
			switch (m_tipo_comando) {
			case tipo_comando::a:
				return m_command.substr(1);
				break;
			case tipo_comando::l:
				return m_command.substr(1, m_command.size() - 2);
				break;
			default:
				return std::string("");
			};
		}

		std::string dest() const
		{
			if (m_tipo_comando != tipo_comando::c)
				return std::string("");

			std::string::size_type find_len = m_command.find("=");
			if (find_len == std::string::npos)
				return std::string("");

			return m_command.substr(0, find_len);
		}

		std::string comp() const
		{
			if (m_tipo_comando != tipo_comando::c)
				return std::string("");

			std::string::size_type find_len = m_command.find("=");
			return m_command.substr(find_len + 1, m_command.find(";") - find_len - 1);
		}

		std::string jump() const
		{
			if (m_tipo_comando != tipo_comando::c)
				return std::string("");

			std::string::size_type find_len = m_command.find(";");
			if (find_len == std::string::npos)
				return std::string("");
			return m_command.substr(find_len + 1);
		}

	private:
		tipo_comando m_tipo_comando = tipo_comando::nenhum;
		uint16_t m_line_num = 0;
		std::ifstream m_file;
		std::string m_command;
	};

	class code {
	public:
		code(const std::string &file, tabelasimbolo *tabelasimbolo)
			: m_file(file, std::ofstream::out),
			  m_tabelasimbolo(tabelasimbolo)
		{
		}

		~code()
		{
			if (m_file.is_open())
				m_file.close();
		}

		void c_instruction(const std::string &dest, const std::string &comp, const std::string &jump)
		{
			m_file << "111" << std::bitset<7>(m_comp[comp])
			       << std::bitset<3>(m_dest[dest])
			       << std::bitset<3>(m_jump[jump])
			       << std::endl;
		}

		void a_instruction(const std::string &symbol)
		{
			uint16_t endereco;
			try {
				if ((endereco = std::stoi(symbol)));
			} catch(std::invalid_argument) {
				if (!m_tabelasimbolo->conteudo(symbol))
					m_tabelasimbolo->add_var(symbol);

				endereco = m_tabelasimbolo->endereco(symbol);
			}
			m_file << std::setfill('0') << std::setw(16) << std::bitset<16>(endereco) << std::endl;
		}

	private:
		std::ofstream m_file;
		tabelasimbolo *m_tabelasimbolo;

		std::map<std::string, uint16_t> m_dest = {
			{"",    0b000},
			{"M",   0b001},
			{"D",   0b010},
			{"MD",  0b011},
			{"A",   0b100},
			{"AM",  0b101},
			{"AD",  0b110},
			{"AMD", 0b111},
		};
		std::map<std::string, uint16_t> m_comp = {
			{"0",   0b0101010},
			{"1",   0b0111111},
			{"-1",  0b0111010},
			{"D",   0b0001100},
			{"A",   0b0110000},
			{"!D",  0b0001101},
			{"!A",  0b0110001},
			{"-D",  0b0001111},
			{"-A",  0b0110011},
			{"D+1", 0b0011111},
			{"A+1", 0b0110111},
			{"D-1", 0b0001110},
			{"A-1", 0b0110010},
			{"D+A", 0b0000010},
			{"D-A", 0b0010011},
			{"A-D", 0b0000111},
			{"D&A", 0b0000000},
			{"D|A", 0b0010101},
			{"M",   0b1110000},
			{"!M",  0b1110001},
			{"-M",  0b1110011},
			{"M+1", 0b1110111},
			{"M-1", 0b1110010},
			{"D+M", 0b1000010},
			{"D-M", 0b1010011},
			{"M-D", 0b1000111},
			{"D&M", 0b1000000},
			{"D|M", 0b1010101},
		};
		std::map<std::string, uint16_t> m_jump = {
			{"",    0b000},
			{"JGT", 0b001},
			{"JEQ", 0b010},
			{"JGE", 0b011},
			{"JLT", 0b100},
			{"JNE", 0b101},
			{"JLE", 0b110},
			{"JMP", 0b111},
		};
	};

} // namespace hacker

int main(int argc, char *argv[])
{
	hacker::tabelasimbolo tabelasimbolo;
	std::string nome_arquivo(argv[1]);
	std::string hack_nome_arquivo(nome_arquivo.substr(0, nome_arquivo.rfind(".")).append(".hack"));
    std::cout << "abrindo arquivo para traducao " << std::endl;
	hacker::parser p(nome_arquivo);
    std::cout << "arquivo aberto com sucesso " << std::endl;
    std::cout << "traduzindo para hack " << std::endl;
	hacker::code c(hack_nome_arquivo, &tabelasimbolo);

	while (p.mais_comandos()) {
		p.avancar();
		if (p.command() == hacker::parser::tipo_comando::l)
			tabelasimbolo.add_label(p.symbol(), p.line());
	}

	p.reset();

	while (p.mais_comandos()) {
		p.avancar();
		if (p.command() == hacker::parser::tipo_comando::a)
			c.a_instruction(p.symbol());
		else if (p.command() == hacker::parser::tipo_comando::c)
			c.c_instruction(p.dest(), p.comp(), p.jump());
	}

	std::cout << "traducao feita com sucesso para o arquivo: " << hack_nome_arquivo << std::endl;

	return 0;
}
