#include "gen.h"

void prompt() {
	string inp;
	Parser p = Parser();
	while (std::getline(std::cin, inp)) {
		auto t = p.parse(inp);
		if (t) {
			std::cout << get_v(t) << std::endl;
		} else {
			std::cout << "sorry" << std::endl;
		}
	}
}

void test_(Parser& p, const string& s, int expected) {
	auto t = p.parse(s);
	assert(t != nullptr);
	assert(get_v(t) == expected);
}

void test_err(Parser& p, const string& s) {
	auto t = p.parse(s);
	assert(t == nullptr);
}

void test() {
	Parser p = Parser();
	test_(p, "0", 0);
	test_(p, "-5", -5);
	test_(p, "--5", 5);
	test_(p, "-(-5)", 5);

	test_(p, "2+2*2", 6);
	test_(p, "2-2-2-2-2-2-2", -10);
	test_(p, "2+2+2+2+2+2+2", 14);
	test_(p, "-(2+2*2) * (2-2*2)", 12);
	test_(p, "-(2+2*2) * -(2-2*2)", -12);

	test_err(p, "");
	test_err(p, "05");
	test_err(p, "+-5");
	test_err(p, "18+");
	test_err(p, "18+5*+5");
	test_err(p, "(2+2)(2+2)");
	test_err(p, "(2+2)*(x+2)");
	test_err(p, "((2+2)*(2+2)+5*(1+1)");
	test_err(p, "2+2 2");

	std::cout << "Tests ok\n";
}

int main() {
	test();
	prompt();
	return 0;
}
