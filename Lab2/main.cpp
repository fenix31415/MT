#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <unordered_map>
#include <fstream>
#include <cassert>

using std::string;
using std::vector;
using std::stack;


enum class Token { VAR, NAME, COLON, ARRAY, LBRACKET, RBRACKET, END, ERR, MAP, COMMA };

class Lexer {
public:
	Lexer(const string& s): source(s), current(s.begin()) {};
	char cur_char() {
		if (current == source.end()) return '\0';
		return *current;
	}
	std::pair<Token, string> next_token() {
		while (isspace(cur_char())) {
			next_char();
		}
		if (cur_char() == '\0') return { Token::END, "" };

		char cur = cur_char();
		switch (cur) {
		case ':':
			next_char();
			return { Token::COLON, "" };
		case '<':
			next_char();
			return { Token::LBRACKET, "" };
		case '>':
			next_char();
			return { Token::RBRACKET, "" };
		case ',':
			next_char();
			return { Token::COMMA, "" };
		default:
			break;
		}

		if (!isalpha(cur)) {
			return { Token::ERR,
				string("Unexpected char '") + cur + "' at " + std::to_string(cur_pos()) };
		}

		string s;
		do {
			s.push_back(cur_char());
			next_char();
		} while (isalnum(cur_char()));

		if (s == "var") return { Token::VAR, s };
		if (s == "Array") return { Token::ARRAY, s };
		if (s == "Map") return { Token::MAP, s };
		return { Token::NAME, s };
	}
	size_t cur_pos() {
		return current - source.begin();
	}
	const string& get_source() {
		return source;
	};

private:
	string::const_iterator current;
	const string& source;

	char next_char() {
		if (current == source.end()) return '\0';
		return *(current++);
	}
};

class Node {
public:
	Node(string type, string data = "") : type(type), data(data) {};
	bool operator==(string s) {
		return type == s;
	}

	void add_ch(Node* node) {
		ch.push_back(node);
	}
	string print() {
		return print_(0).first;
	}
	void set_data(string s) {
		data = s;
	}

private:
	std::pair<string, int> print_(int id) {
		string ans = "";
		string var_name_root = "v_" + std::to_string(id++);
		ans += var_name_root + "[label = \"" + get_label() + "\"];\n";
		for (int i = ch.size() - 1; i >= 0; --i) {
			auto c = ch[i];
			string var_name_c = "v_" + std::to_string(id);
			auto t = c->print_(id);
			id = t.second;
			ans += t.first;

			ans += var_name_root + " -- " + var_name_c + " ;\n";
		}
		return {ans, id};
	}
	string get_label() {
		string ans = type;
		if (data != "") {
			ans += "\\n'" + data + "'";
		}
		return ans;
	}

	string type, data;
	std::vector<Node*> ch;
};

class Parser {
public:
	Parser(const string& s) : l(s) {};
	Node* parse() {
		Node* root = new Node("D");
		s.push(root);
		curtoken_info = l.next_token();

		while (!s.empty()) {
			auto cur = s.top();
			s.pop();

			if (*cur == "D") {
				push_and_add(cur, "TT");
				push_and_add(cur, "C");
				push_and_add(cur, "N");
				push_and_add(cur, "V");
			}

			if (*cur == "TT") {
				if (curtoken_info.first == Token::ARRAY) {
					push_and_add(cur, ">");
					push_and_add(cur, "TT");
					push_and_add(cur, "<");
					push_and_add(cur, "A");
				} else if (curtoken_info.first == Token::MAP) {
					push_and_add(cur, ">");
					push_and_add(cur, "TT");
					push_and_add(cur, "COM");
					push_and_add(cur, "TT");
					push_and_add(cur, "<");
					push_and_add(cur, "M");
				} else if (curtoken_info.first == Token::NAME) {
					accept(cur);
				} else {
					print_err_expected("AM");
					return nullptr;

				}
			}

			/*if (*cur == "T") {
				std::cout << "Q";
				if (curtoken_info.first == Token::NAME) {
					accept(cur);
				} else if (curtoken_info.first == Token::ARRAY) {
					push_and_add(cur, ">");
					push_and_add(cur, "T");
					push_and_add(cur, "<");
					push_and_add(cur, "A");
				} else {
					print_err_expected("T");
					return nullptr;
				}
			}*/

			// if cur in VNCA<T>
			for (auto& i : expectation_data) {
				if (*cur == i.first) {
					if (curtoken_info.first != i.second) {
						print_err_expected(i.first);
						return nullptr;
					}
					accept(cur);
					break;
				}
			}
		}
		if (curtoken_info.first != Token::END) {
			print_err("expected end of line\n");
			return nullptr;
		}
		return root;
	}

private:
	Lexer l;
	string errmsg;
	stack<Node*> s;
	size_t last_ind = 0;
	std::pair<Token, string> curtoken_info;
	static const std::vector<std::pair<string, Token>> expectation_data;
	static const std::unordered_map<string, string> data;

	void push_and_add(Node* cur, string type, string data="") {
		Node* t = new Node(type, data);
		s.push(t);
		cur->add_ch(t);
	}
	void print_err_expected(string what) {
		size_t len = l.get_source().size();
		while (last_ind < len && isspace(l.get_source()[last_ind])) ++last_ind;
		print_err("expected '" + data.at(what) + "' at " + std::to_string(last_ind) + "\n" + l.get_source());
	}
	void print_err(string errmsg) {
		std::cout << errmsg << "\n" << l.get_source() << "\n";
		for (int i = 0; i < last_ind; i++) {
			std::cout << "-";
		}std::cout << "^\n\n";
	}
	void accept(Node* cur) {
		cur->set_data(curtoken_info.second);
		last_ind = l.cur_pos();
		curtoken_info = l.next_token();
	}
};

const std::vector<std::pair<string, Token>> Parser::expectation_data = {
	{"V", Token::VAR},
	{"N", Token::NAME},
	{"C", Token::COLON},
	{"COM", Token::COMMA},
	{"A", Token::ARRAY},
	{"M", Token::MAP},
	{">", Token::RBRACKET},
	{"<", Token::LBRACKET},
};

const std::unordered_map<string, string> Parser::data = {
	{"V", "var"},
	{"N", "*some_name*"},
	{"T", "*some_type_name* or *array*"},
	{"C", ":"},
	{"COM", ","},
	{"AM", "Array or Map"},
	{">", ">"},
	{"<", "<"},
};

void draw(string s) {
	Parser p(s);
	auto tree = p.parse();
	if (!tree) return;
	std::ofstream outfile("out.txt");
	outfile << "graph \"\"{" << "\n";
	outfile << tree->print() << "\n}";
	outfile.close();
	system("dot  -Tpng out.txt -o out.png");
	system("\"c:\\Program Files\\IrfanView\\i_view64.exe\" out.png");
}

Node* try_parse(string s) {
	Parser p(s);
	return p.parse();
}

void test_ok(string s) {
	std::cout << "'testing: " << s << "': ";
	assert(try_parse(s) != nullptr);
	std::cout << "ok\n";
}

void test_nok(string s) {
	std::cout << "'testing: " << s << "': \n";
	assert(try_parse(s) == nullptr);
	std::cout << "ok\n";
}

void test() {
	test_ok("var name:Array<int>");
	test_ok("	    var\n	   \nname			  :			\n	   Array	 <     int    >		   ");
	test_ok("	    var\nname			  :\t\t\t\n	   Array<    int    >	\n\n\n   ");
	test_ok("var f314Name: Array <MyTyPe>");
	
	test_nok("var endLine:Array<int");
	test_nok("var array: array <int>");
	test_nok("var colon ; Array<int>");
	test_nok("var colon : Array<>");
	test_nok("var colon : Array<int> some_staff");
	test_nok("var colon :");
	test_nok("var c");
	test_nok("var");
	test_nok("v");
	

	test_ok("var name:Array<Array<Int2>>");
	test_ok("var name:Array<Array<Int2> >");
	test_ok("var name:Array<Array<Int2>>");

	test_nok("var name:Array<Array<Int_32>>");
	test_nok("var name:Array<Array<var name:Array<Array<Int32>>>>");
	test_nok("var name:Array<array<Int32>>");
	test_nok("var name:Array<Array<Int32>");
	test_nok("var name:Array<name1:Array<Int32>>");
	test_nok("var name:Array<:Array<Int32>>");
}
#include <filesystem>
namespace fs = std::filesystem;
int main(int argc, char* args[]) {
	//test();
	//draw("var name:Array<Array<Array<Array<Array<Array<Array<Array<Array<Array<Array<Array<Array<Array<type>>>>>>>>>>>>>>");
	//draw("var name:Map<Array<type1>, Map<type2,Map<Array<type3>,type4,>>>");
	fs::create_directories(".\\q");
	std::cout << "Q1\n";
	std::ofstream out = std::ofstream(".\\q\\mine.txt");
	out << "Q";
	if (!out)
		std::cout << "Q2\n";
	out.close();
}
/*
1) D -> var n:Array<T>
2) T -> n
3) T -> Array <T>
*/