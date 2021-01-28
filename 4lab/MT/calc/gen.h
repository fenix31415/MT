#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <unordered_map>
#include <set>
#include <fstream>
#include <algorithm>
#include <cassert>
#include <regex>


using std::vector;
using std::string;
using std::stack;
using std::pair;


/*
python ReadGrammar.py grammar_calc.txt && cl /EHsc /std:c++17 MT/main.cpp && main.exe
*/
enum class Token { LP, RP, PLUS, MINUS, MUL, NUMBER, ABS, SPACE, _END, ERR };

class Node {
public:
	Node() : ch(), name("") {}

	Node(string name) : ch(), name(name) {}

	void add_ch(Node* node) {
		ch.push_back(node);
	}

private:
	string name;
	std::vector<Node*> ch;
};

class Node_E : public Node {
public:
	Node_E() : Node("E") {};
	int v;
};
class Node_F : public Node {
public:
	Node_F() : Node("F") {};
	int v;
};
class Node_G : public Node {
public:
	Node_G() : Node("G") {};
	int v;
};
class Node_H : public Node {
public:
	Node_H() : Node("H") {};
	int v;
};
class Node_MinusOp : public Node {
public:
	Node_MinusOp() : Node("MinusOp") {};
	int v;
};
class Node_MulOp : public Node {
public:
	Node_MulOp() : Node("MulOp") {};
	int v;
};
class Node_PlusOp : public Node {
public:
	Node_PlusOp() : Node("PlusOp") {};
	int v;
};
class Node_T : public Node {
public:
	Node_T() : Node("T") {};
	int v;
};
class TokenInfo {
public:
	Token token;
	std::regex r;
	TokenInfo(Token token, const string& reg) : token(token), r(reg) {}
};

class Lexer {
public:
	Lexer() {};
	Lexer(const string& str): s(str) {
		token_patterns.push_back({ Token::LP, "[(]" });
		token_patterns.push_back({ Token::RP, "[)]" });
		token_patterns.push_back({ Token::PLUS, "[\\+]" });
		token_patterns.push_back({ Token::MINUS, "[-]" });
		token_patterns.push_back({ Token::MUL, "[\\*]" });
		token_patterns.push_back({ Token::NUMBER, "[1-9]+[0-9]*|0" });
		token_patterns.push_back({ Token::ABS, "[|]" });
		skip_patterns.push_back({ Token::SPACE, "[ \n\r]+" });
		auto it = s.begin();
		while (it != s.end()) {
			auto t = longest(it, token_patterns);
			if (t.first != "") {
				tokens.push_back({ t.second, t.first });
			} else {
				if (longest(it, skip_patterns).first == "") {
					tokens.push_back({ Token::ERR, "unexpected symbol" });
					return;
				}
			}
		}
		tokens.push_back({ Token::_END, "" });
	}
	Token cur_token() {
		return tokens[current].first;
	}
	string cur_token_str() {
		return tokens[current].second;
	}
	void next_token() {
		++current;
	}
private:
	vector<TokenInfo> token_patterns = vector<TokenInfo>();
	vector<TokenInfo> skip_patterns = vector<TokenInfo>();
	vector<std::pair<Token, string>> tokens;
	int current = 0;
	string s;
	std::pair<string, Token> longest(string::iterator& l, vector<TokenInfo> v) {
		for (auto& p : v) {
			const std::regex r(p.r);
			for (auto i = s.end(); i != l; i--) {
				if (std::regex_match(l, i, r)) {
					string ans = s.substr(l - s.begin(), i - l);l = i;return { ans, p.token };
				}
			}
		}
		return { "", Token::_END };
	}
};

class Parser {
public:
	Parser() {};
	Node_E* parse(const string& s) {
		l = Lexer(s);
		try {
			Node_E* node = _E();
			if (l.cur_token() != Token::_END) {
				std::cout<<"expected eof"; return nullptr;
			}
			return node;
		} catch (const std::runtime_error & e) {
			std::cout << e.what() << '\n'; return nullptr;
		}
	}
private:
	Lexer l;

	Node_E* _E();
	Node_F* _F();
	Node_G* _G(int a);
	Node_H* _H(int a);
	Node_MinusOp* _MinusOp(int a, int b);
	Node_MulOp* _MulOp(int a, int b);
	Node_PlusOp* _PlusOp(int a, int b);
	Node_T* _T();
};

Node_E* Parser::_E() {
	Node_E* res = new Node_E();
	switch(l.cur_token()) {
	case Token::ABS:
	case Token::LP:
	case Token::MINUS:
	case Token::NUMBER:
	{
		Node_T* n0 = _T();
		res->add_ch(n0);
		int u = n0->v;
		Node_G* n1 = _G(u);
		res->add_ch(n1);
		res->v = n1->v;
		return res;
	}
	}
	throw std::runtime_error("unexpected token");
}

Node_F* Parser::_F() {
	Node_F* res = new Node_F();
	switch(l.cur_token()) {
	case Token::NUMBER:
	{
		if (l.cur_token() != Token::NUMBER) throw std::runtime_error("unexpected token");
		res->add_ch(new Node("NUMBER"));
		res->v = std::stoi(l.cur_token_str());
		l.next_token();
		return res;
	}
	case Token::MINUS:
	{
		if (l.cur_token() != Token::MINUS) throw std::runtime_error("unexpected token");
		res->add_ch(new Node("MINUS"));
		
		l.next_token();
		Node_F* n0 = _F();
		res->add_ch(n0);
		res->v = -(n0->v);
		return res;
	}
	case Token::LP:
	{
		if (l.cur_token() != Token::LP) throw std::runtime_error("unexpected token");
		res->add_ch(new Node("LP"));
		
		l.next_token();
		Node_E* n0 = _E();
		res->add_ch(n0);
		res->v = n0->v;
		if (l.cur_token() != Token::RP) throw std::runtime_error("unexpected token");
		res->add_ch(new Node("RP"));
		
		l.next_token();
		return res;
	}
	case Token::ABS:
	{
		if (l.cur_token() != Token::ABS) throw std::runtime_error("unexpected token");
		res->add_ch(new Node("ABS"));
		
		l.next_token();
		Node_E* n0 = _E();
		res->add_ch(n0);
		res->v = abs(n0->v);
		if (l.cur_token() != Token::ABS) throw std::runtime_error("unexpected token");
		res->add_ch(new Node("ABS"));
		
		l.next_token();
		return res;
	}
	}
	throw std::runtime_error("unexpected token");
}

Node_G* Parser::_G(int a) {
	Node_G* res = new Node_G();
	switch(l.cur_token()) {
	case Token::ABS:
	case Token::RP:
	case Token::_END:
	{
		res->v = a;
		return res;
	}
	case Token::PLUS:
	{
		if (l.cur_token() != Token::PLUS) throw std::runtime_error("unexpected token");
		res->add_ch(new Node("PLUS"));
		
		l.next_token();
		Node_T* n0 = _T();
		res->add_ch(n0);
		int u = n0->v;
		Node_PlusOp* n1 = _PlusOp(a,u);
		res->add_ch(n1);
		int acc = n1->v;
		Node_G* n2 = _G(acc);
		res->add_ch(n2);
		res->v = n2->v;
		return res;
	}
	case Token::MINUS:
	{
		if (l.cur_token() != Token::MINUS) throw std::runtime_error("unexpected token");
		res->add_ch(new Node("MINUS"));
		
		l.next_token();
		Node_T* n0 = _T();
		res->add_ch(n0);
		int u = n0->v;
		Node_MinusOp* n1 = _MinusOp(a,u);
		res->add_ch(n1);
		int acc = n1->v;
		Node_G* n2 = _G(acc);
		res->add_ch(n2);
		res->v = n2->v;
		return res;
	}
	}
	throw std::runtime_error("unexpected token");
}

Node_H* Parser::_H(int a) {
	Node_H* res = new Node_H();
	switch(l.cur_token()) {
	case Token::ABS:
	case Token::MINUS:
	case Token::PLUS:
	case Token::RP:
	case Token::_END:
	{
		res->v = a;
		return res;
	}
	case Token::MUL:
	{
		if (l.cur_token() != Token::MUL) throw std::runtime_error("unexpected token");
		res->add_ch(new Node("MUL"));
		
		l.next_token();
		Node_F* n0 = _F();
		res->add_ch(n0);
		int u = n0->v;
		Node_MulOp* n1 = _MulOp(a,u);
		res->add_ch(n1);
		int acc = n1->v;
		Node_H* n2 = _H(acc);
		res->add_ch(n2);
		res->v = n2->v;
		return res;
	}
	}
	throw std::runtime_error("unexpected token");
}

Node_MinusOp* Parser::_MinusOp(int a, int b) {
	Node_MinusOp* res = new Node_MinusOp();
	switch(l.cur_token()) {
	case Token::ABS:
	case Token::MINUS:
	case Token::PLUS:
	case Token::RP:
	case Token::_END:
	{
		res->v = a - b;
		return res;
	}
	}
	throw std::runtime_error("unexpected token");
}

Node_MulOp* Parser::_MulOp(int a, int b) {
	Node_MulOp* res = new Node_MulOp();
	switch(l.cur_token()) {
	case Token::ABS:
	case Token::MINUS:
	case Token::MUL:
	case Token::PLUS:
	case Token::RP:
	case Token::_END:
	{
		res->v = a * b;
		return res;
	}
	}
	throw std::runtime_error("unexpected token");
}

Node_PlusOp* Parser::_PlusOp(int a, int b) {
	Node_PlusOp* res = new Node_PlusOp();
	switch(l.cur_token()) {
	case Token::ABS:
	case Token::MINUS:
	case Token::PLUS:
	case Token::RP:
	case Token::_END:
	{
		res->v = a + b;
		return res;
	}
	}
	throw std::runtime_error("unexpected token");
}

Node_T* Parser::_T() {
	Node_T* res = new Node_T();
	switch(l.cur_token()) {
	case Token::ABS:
	case Token::LP:
	case Token::MINUS:
	case Token::NUMBER:
	{
		Node_F* n0 = _F();
		res->add_ch(n0);
		int u = n0->v;
		Node_H* n1 = _H(u);
		res->add_ch(n1);
		res->v = n1->v;
		return res;
	}
	}
	throw std::runtime_error("unexpected token");
}

int get_v(Node_E* node) {
	return node->v;
}

