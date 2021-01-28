%{
    #include <string>
    #include <iostream>

    using namespace std;

    #define YYSTYPE string
    #include "parser.tab.h"
%}

identifier      [_a-zA-Z][_a-zA-Z0-9]*
intnumber       0|[1-9][0-9]*
eqs             "=="|"!="
or              "||"
eq_op           "="|"*="|"/="|"%="|"+="|"-="|"^="
rel             "<"|">"|"<="|">="

spec            "static"|"auto"
type            "void"|"char"|"short"|"int"|"long"|"float"|"double"|"signed"|"unsigned"|"const"|"volatile"

string_single   "\'"[^'\\]*"\'"
string_double   "\""[^"\\]*"\""
char            "\'"[^"'\\]"\'"
bool            "true"|"false"
comm            "/*"[^\*]*"*/"

%%

{comm}          { yylval = yytext; return COMMENT; }
[;]             { yylval = yytext; return SEMICOLON; }
continue|break  { yylval = yytext; return MOVE; }
return          { yylval = yytext; return RETN; }
case            { yylval = yytext; return CASE; }
default         { yylval = yytext; return DEFAULT; }
if              { yylval = yytext; return IF; }
else            { yylval = yytext; return ELSE; }
switch          { yylval = yytext; return SWITCH; }
while           { yylval = yytext; return WHILE; }
do              { yylval = yytext; return DO; }
for             { yylval = yytext; return FOR; }
{spec}          { yylval = yytext; return TYPE_SPEC; } // static auto
{type}          { yylval = yytext; return TYPE; }      // const void unsigned
{bool}          { yylval = yytext; return BOOL; }
{identifier}    { yylval = yytext; return ID; }
{char}          { yylval = yytext; return CHAR; }
{string_single} { yylval = yytext; return STRING_S; }
{string_double} { yylval = yytext; return STRING_D; }
{eq_op}         { yylval = yytext; return EQ; }
{eqs}           { yylval = yytext; return EQS; }
{or}            { yylval = yytext; return OR; }
[\^]            { yylval = yytext; return XOR; }
[&]             { yylval = yytext; return AMP; }
[~!]            { yylval = yytext; return UNAR; }
{rel}           { yylval = yytext; return REL; }
[+-]            { yylval = yytext; return ADD; }
[/%*]           { yylval = yytext; return MUL; }
[[]             { yylval = yytext; return LQB; }
[]]             { yylval = yytext; return RQB; }
[(]             { yylval = yytext; return LB; }
[)]             { yylval = yytext; return RB; }
[{]             { yylval = yytext; return LFB; }
[}]             { yylval = yytext; return RFB; }
[,]             { yylval = yytext; return COMMA; }
[:]             { yylval = yytext; return COLON; }
{intnumber}     { yylval = yytext; return NUM; }
\n\r|\r\n|\n|\r { /* skip empty line */ }
[ \t]+          ; /* skip whitespaces */
.               {
                    yylval = yytext;
                    cout<<yylval<<endl;
                    return OTHER;
                }
                
%%