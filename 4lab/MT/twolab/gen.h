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
python ReadGrammar.py grammar_2lab.txt && cl /EHsc /std:c++17 MT/main.cpp && main.exe
*/
enum class Token { LP, RP, COLON, VAR, ARRAY, NAME, SPACE, _END, ERR };

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
	;
};
class Node_T : public Node {
public:
	Node_T() : Node("T") {};
	;
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
		token_patterns.push_back({ Token::LP, "[<]" });
		token_patterns.push_back({ Token::RP, "[>]" });
		token_patterns.push_back({ Token::COLON, "[:]" });
		token_patterns.push_back({ Token::VAR, "var" });
		token_patterns.push_back({ Token::ARRAY, "Array" });
		token_patterns.push_back({ Token::NAME, "[_a-zA-Z][_a-zA-Z0-9]*" });
		skip_patterns.push_back({ Token::SPACE, "[ \n\r\t]+" });
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
	Node_T* _T();
};

Node_E* Parser::_E() {
	Node_E* res = new Node_E();
	switch(l.cur_token()) {
	case Token::VAR:
	{
		if (l.cur_token() != Token::VAR) throw std::runtime_error("unexpected token");
		res->add_ch(new Node("VAR"));
		
		l.next_token();
		if (l.cur_token() != Token::NAME) throw std::runtime_error("unexpected token");
		res->add_ch(new Node("NAME"));
		
		l.next_token();
		if (l.cur_token() != Token::COLON) throw std::runtime_error("unexpected token");
		res->add_ch(new Node("COLON"));
		
		l.next_token();
		if (l.cur_token() != Token::ARRAY) throw std::runtime_error("unexpected token");
		res->add_ch(new Node("ARRAY"));
		
		l.next_token();
		if (l.cur_token() != Token::LP) throw std::runtime_error("unexpected token");
		res->add_ch(new Node("LP"));
		
		l.next_token();
		Node_T* n0 = _T();
		res->add_ch(n0);
		
		if (l.cur_token() != Token::RP) throw std::runtime_error("unexpected token");
		res->add_ch(new Node("RP"));
		
		l.next_token();
		return res;
	}
	}
	throw std::runtime_error("unexpected token");
}

Node_T* Parser::_T() {
	Node_T* res = new Node_T();
	switch(l.cur_token()) {
	case Token::NAME:
	{
		if (l.cur_token() != Token::NAME) throw std::runtime_error("unexpected token");
		res->add_ch(new Node("NAME"));
		
		l.next_token();
		return res;
	}
	case Token::ARRAY:
	{
		if (l.cur_token() != Token::ARRAY) throw std::runtime_error("unexpected token");
		res->add_ch(new Node("ARRAY"));
		
		l.next_token();
		if (l.cur_token() != Token::LP) throw std::runtime_error("unexpected token");
		res->add_ch(new Node("LP"));
		
		l.next_token();
		Node_T* n0 = _T();
		res->add_ch(n0);
		
		if (l.cur_token() != Token::RP) throw std::runtime_error("unexpected token");
		res->add_ch(new Node("RP"));
		
		l.next_token();
		return res;
	}
	}
	throw std::runtime_error("unexpected token");
}


