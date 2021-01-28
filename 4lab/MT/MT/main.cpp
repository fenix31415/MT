#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <unordered_map>
#include <set>
#include <fstream>
#include <algorithm>
#include <cassert>
#include <filesystem>

using std::vector;
using std::string;
using std::stack;
using std::pair;


typedef std::unordered_map<string, vector<string>> NtoVec;
typedef std::unordered_map<string, std::set<string>> NtoSet;

#include "gen1.txt"

class Item {
public:
	Item(string name, bool b=true) :name(name), is_term(b) {};
	bool operator==(const string s) const {
		return name == s;
	}

	string name;
	bool is_term;
};

class Grammar {
public:
	class Rule {
	public:
		Rule(Item left) :left(left) {};
		Rule(Item left, vector<Item> right, vector<string> codes, vector<string> params) :left(left), right(right), codes(codes), params(params) {};
		void add_r(string name, bool is_term) {
			right.push_back(Item(name, is_term));
		}
		string get_left() const {
			return left.name;
		}
		bool is_term() const {
			return left.is_term;
		}
		vector<Item> right;
		vector<string> codes, params;
	private:
		Item left;
	};

	Grammar(NtoVec inh, NtoVec sint, vector<std::pair<std::pair<string, string>, bool>> toks, vector<Rule> r, string st) :rules(std::move(r)), start_no_term(st), tokens(toks), inh(inh), sint(sint) {
		for (auto& rule : rules) {
			no_terms.insert(rule.get_left());
		}
	};
	void add_rule(Rule rule) {
		rules.push_back(rule);
		no_terms.insert(rule.get_left());
	};
	void run(string path) {
		init_FF();
		std::filesystem::create_directories(path);
		out = std::ofstream(path + "gen.h");

		put("#pragma once");
		put("#include <iostream>\n#include <string>\n#include <vector>\n#include <stack>\n#include <unordered_map>\n#include <set>\n#include <fstream>\n#include <algorithm>\n#include <cassert>\n#include <regex>\n\n");
		put("using std::vector;\nusing std::string;\nusing std::stack;\nusing std::pair;\n\n");

		put(get_header());

		print_tokens();

		put("class Node {\npublic:"); ++depth;
		put("Node() : ch(), name(\"\") {}\n");
		put("Node(string name) : ch(), name(name) {}\n");
		put("void add_ch(Node* node) {"); ++depth;
		put("ch.push_back(node);"); --depth;
		put("}\n\nprivate:");
		put("string name;");
		put("std::vector<Node*> ch;\n};\n"); --depth;

		for (auto& no_term : no_terms) {
			put("class Node_" + no_term + " : public Node {");
			put("public:"); ++depth;
			put("Node_" + no_term + "() : Node(\"" + no_term + "\") {};");
			for (auto& param : sint[no_term]) {
				put(param + ";");
			}
			--depth; put("};");
		}

		print_lexer();

		print_parser();

		for (auto s : sint[start_no_term]) {
			if (s == "") continue;
			auto spl = s.rfind(" ");
			string field(s.substr(spl + 1));
			string field_type = s.substr(0, spl);
			put(field_type + " get_" + field + "(Node_" + start_no_term + "* node) {"); ++depth;
			put("return node->" + field + ";");
			--depth; put("}");
		}

		put(get_codes());
		
		out.close();
	}
private:
	const static string EPS;
	std::ofstream out;
	int depth = 0;

	vector<Rule> rules;
	std::set<string> no_terms;
	string start_no_term;
	NtoVec inh, sint;
	NtoSet first, follow;
	vector<std::pair<std::pair<string, string>, bool>> tokens;

	void print_lexer() {
		put("class TokenInfo {");
		put("public:"); ++depth;
		put("Token token;");
		put("std::regex r;");
		put("TokenInfo(Token token, const string& reg) : token(token), r(reg) {}");
		--depth; put("};\n");


		put("class Lexer {");
		put("public:"); ++depth;
		put("Lexer() {};");
		put("Lexer(const string& str): s(str) {"); ++depth;
		for (const auto& i : tokens) {
			string what = "token_patterns";
			if (i.second) what = "skip_patterns";
			put(what + ".push_back({ Token::" + i.first.first + ", \"" + i.first.second + "\" });");
		}
		put("auto it = s.begin();");
		put("while (it != s.end()) {"); ++depth;
		put("auto t = longest(it, token_patterns);");
		put("if (t.first != \"\") {"); ++depth;
		put("tokens.push_back({ t.second, t.first });");
		--depth; put("} else {"); ++depth;
		put("if (longest(it, skip_patterns).first == \"\") {"); ++depth;
		// put("assert(false);"); -- unexpected symbol
		put("tokens.push_back({ Token::ERR, \"unexpected symbol\" });");
		put("return;");
		--depth; put("}"); --depth; put("}"); --depth; put("}"); 
		put("tokens.push_back({ Token::_END, \"\" });");
		--depth; put("}");

		put("Token cur_token() {"); ++depth; /*put("if (current == tokens.size()) return Token::_END;");*/ put("return tokens[current].first;"); --depth; put("}");
		put("string cur_token_str() {"); ++depth; put("return tokens[current].second;"); --depth; put("}");
		put("void next_token() {"); ++depth; put("++current;"); --depth; put("}");

		--depth; put("private:"); ++depth;
		put("vector<TokenInfo> token_patterns = vector<TokenInfo>();");
		put("vector<TokenInfo> skip_patterns = vector<TokenInfo>();");
		put("vector<std::pair<Token, string>> tokens;");
		put("int current = 0;");
		put("string s;");

		put("std::pair<string, Token> longest(string::iterator& l, vector<TokenInfo> v) {"); ++depth;
		put("for (auto& p : v) {"); ++depth;
		put("const std::regex r(p.r);");
		put("for (auto i = s.end(); i != l; i--) {"); ++depth;
		put("if (std::regex_match(l, i, r)) {"); ++depth;
		put("string ans = s.substr(l - s.begin(), i - l);l = i;return { ans, p.token };");
		--depth; put("}"); --depth; put("}"); --depth; put("}"); put("return { \"\", Token::_END };"); --depth; put("}");

		--depth; put("};\n");

	}
	void print_parser() {
		put("class Parser {\npublic:"); ++depth;
		put("Parser() {};");
		put("Node_" + start_no_term + "* parse(const string& s) {"); ++depth;
		put("l = Lexer(s);");
		put("try {"); ++depth;
		put("Node_" + start_no_term + "* node = _" + start_no_term + "();");
		put("if (l.cur_token() != Token::_END) {"); ++depth;
		put("std::cout<<\"expected eof\"; return nullptr;");
		--depth; put("}"); put("return node;");
		--depth; put("} catch (const std::runtime_error & e) {"); ++depth;
		put("std::cout << e.what() << \'\\n\'; return nullptr;");
		--depth; put("}");
		--depth; put("}");
		--depth; put("private:"); ++depth;
		put("Lexer l;\n");

		for (auto& no_term : no_terms) {
			put("Node_" + no_term + "* _" + no_term + "(" + get_inh(no_term) + ");");
		}

		--depth; put("};\n");

		for (auto& no_term : no_terms) {
			print_no_term(no_term);
		}

	}
	string get_inh(const string& no_term) {
		string ans = "";
		for (const auto& i : inh[no_term]) {
			ans += i;
		}
		return ans;
	}
	void print_no_term(const string& no_term) {
		put("Node_" + no_term + "* Parser::_" + no_term + "(" + get_inh(no_term) + ") {"); ++depth;
		put("Node_" + no_term + "* res = new Node_" + no_term + "();");
		put("switch(l.cur_token()) {");

		for (const auto& rule : rules) {
			if (rule.get_left() != no_term) continue;
			auto tmp = get_first_(rule);
			const std::set<string>& first_ = tmp.first;
			bool has_eps = tmp.second;
			for (const auto& item : first_) {
				put("case Token::" + item + ":");
			}
			put("{"); ++depth;
			if (has_eps) {
				put(rule.codes[0]);
				put("return res;");
				--depth; put("}");
				continue;
			}

			int index = 0;
			for (int ind = 0; ind < rule.right.size(); ++ind) {
				const auto& i = rule.right[ind];
				if (i.is_term) {
					put("if (l.cur_token() != Token::" + i.name + ") throw std::runtime_error(\"unexpected token\");");
					//put("assert(l.cur_token() == Token::" + i.name + ");");
					put("res->add_ch(new Node(\"" + i.name + "\"));");
					put(rule.codes[ind]);
					put("l.next_token();");
				} else {
					put("Node_" + i.name + "* n" + std::to_string(index) + " = _" + i.name + "(" + rule.params[ind] + ");");
					put("res->add_ch(n" + std::to_string(index) + ");");
					put(rule.codes[ind]);
					++index;
				}
			}
			put("return res;");
			--depth; put("}");
		}

		put("}"); put("throw std::runtime_error(\"unexpected token\");");
		--depth; put("}\n");
	}
	void put(const string& s) {
		for (int i = 0; i < depth; i++) {
			out << "\t";
		} out << s << std::endl;
	}
	void print_tokens() {
		out << "enum class Token { ";
		for (auto& t : init_tokens()) {
			out << t.first.first << ", ";
		}
		out << "_END, ERR };\n\n";
	}
	std::pair<std::set<string>, bool> get_first_(const Rule& rule) {
		std::set<string> f;
		std::set<string> first_a = std::set<string>();
		const auto& r = rule.right;
		get_first(r, first_a);
		f.insert(first_a.begin(), first_a.end());
		bool ans = false;
		if (first_a.find(EPS) != first_a.end()) {
			ans = true;
			f.erase(EPS);
			const auto& follow_A = follow[rule.get_left()];
			f.insert(follow_A.begin(), follow_A.end());
		}
		return { f, ans };
	}
	void init_FF() {
		calc_first();
		calc_follow();
	}
	void calc_first() {
		init_set_no_term(first);
		bool changed = true;
		while (changed) {
			changed = false;
			for (const auto& r : rules) {
				auto& f = first[r.get_left()];
				std::set<string> to_add = std::set<string>();
				get_first(r.right, to_add);
				for (const auto& i : to_add) {
					if (f.find(i) == f.end()) {
						changed = true;
						f.insert(i);
					}
				}
			}
		}
	}
	void calc_follow() {
		init_set_no_term(follow);
		follow[start_no_term].insert("_END");
		bool changed = true;
		while (changed) {
			changed = false;
			for (const auto& r : rules) {
				auto& r_right = r.right;
				for (auto it = r_right.begin(); it != r_right.end(); ++it) {
					const auto& no_term = *it;
					if (no_term.is_term) continue;
					auto& f = follow[no_term.name];
					std::set<string> eta_first = std::set<string>();
					get_first(it + 1, r_right.end(), eta_first);
					if (eta_first.find(EPS) != eta_first.end()) {
						eta_first.erase(EPS);
						const auto& follow_A = follow[r.get_left()];
						eta_first.insert(follow_A.begin(), follow_A.end());
					}
					for (const auto& i : eta_first) {
						if (f.find(i) == f.end()) {
							changed = true;
							f.insert(i);
						}
					}
				}
			}
		}
	}
	void get_first(vector<Item>::const_iterator r_b, vector<Item>::const_iterator r_e, std::set<string>& f) {
		if (r_b == r_e) {
			f.insert(EPS);
		}
		for (auto it = r_b; it != r_e; ++it) {
			auto& i = *it;
			if (i == EPS) {
				f.insert(EPS);
			} else if (i.is_term) {
				f.insert(i.name);
				break;
			} else {
				const auto& i_first = first[i.name];
				for (const auto& it : i_first) {
					f.insert(it);
				}
				if (i_first.find(EPS) == i_first.end()) {
					break;
				}
			}
		}
	}
	void get_first(const vector<Item>& r, std::set<string>& f) {
		get_first(r.begin(), r.end(), f);
	}
	void init_set_no_term(std::unordered_map<string, std::set<string>>& set) {
		if (set.size() > 0) return;
		for (const auto& i : rules) {
			if (i.is_term()) continue;
			set[i.get_left()] = std::set<string>();
		}
	}
	void init_vec_no_term(std::unordered_map<string, vector<string>>& v) {
		if (v.size() > 0) return;
		for (const auto& i : rules) {
			if (i.is_term()) continue;
			v[i.get_left()] = vector<string>();
		}
	}
};

const string Grammar::EPS = "EPS";

int main() {
	#include "gen2.txt"
	return 0;
}
