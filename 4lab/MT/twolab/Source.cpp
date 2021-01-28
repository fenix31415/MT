#include "gen.h"


Node_E* try_parse(string s) {
	Parser p = Parser();
	return p.parse(s);
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
	test_ok("var name:Array<Array<Int_32>>");

	test_nok("var name:Array<Array<var name:Array<Array<Int32>>>>");
	test_nok("var name:Array<array<Int32>>");
	test_nok("var name:Array<Array<Int32>");
	test_nok("var name:Array<name1:Array<Int32>>");
	test_nok("var name:Array<:Array<Int32>>");

	std::cout << "tests ok\n";
}

int main() {
	test();
	/*Parser p = Parser();
	auto t = p.parse("");*/
	return 0;
}