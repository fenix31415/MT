%{
    #include <iostream>
    #include <algorithm>
    #include <string>
    
    using namespace std;
    #define YYSTYPE string
    #define YYERROR_VERBOSE 1
    #define DEBUG
    
    int  wrapRet = 1;
    
    int yylex(void);
    extern "C" {
        int yywrap( void ) {
            return wrapRet;
        }
    }
    void yyerror(const char *str) {
        #ifdef DEBUG
          cout << " [@#$$!@#^&^!#$@] RRParserRR: " << str << endl;
        #endif
    }
    
    int depth = 0;
    string make_command(string s) { // padding
      string ans = "";
      for (int i = 0; i < depth; ++i)
        ans += "&emsp;";
      return ans + s;
    }
    
    
    
    /////////////////////////
    //        DECOR
    /////////////////////////
    
    
    string make_char(string c) {
      return "<i style=\"font-family:cursive;color:blue;\">" + c + "</i>";
      //return c + "";
    }
    string make_decimal(string c) {
      return "<em style=\"font-family:monospace;color:Orange;font-size:15px;\">" + c + "</em>";
      //return c + "";
    }
    string make_bool(string c) {
      return "<i style=\"font-family:monospace;color:DodgerBlue;\">" + c + "</i>";
      //return c + "";
    }
    string make_string(string c) {
      return "<b style=\"font-family:cursive;color:blue;\">" + c + "</b>";
      //return c + "";
    }
    string make_initializer_list(string l) {
      return "<b style=\"font-family:monospace;color:MediumOrchid;font-size:20px;\">{</b> " + l + " <b style=\"font-family:monospace;color:MediumOrchid;font-size:20px;\">}</b>";
    }
    string make_fun_type(string t, string a2, string a3) {
      if (a3 == ";") return "<b style=\"font-family:verdana;color:red;\">" + t + "</b> " + a2 + ";";  // type of var
      return "<i style=\"font-family:fantasy;color:red;\">" + t + "</i> " + a2 + " " + a3;   // type of fun
    }
    string make_fun_decl(string name, string argss) {
      return "<b style=\"font-size:20px;\">" + name + "</b>" + "(" + argss + ")";
      //return name + " (" + argss + ")";
    }
    string make_param_decl(string name, string argss) {
      return "<i style=\"font-family:times;color:green;\">" + name + "</i>" + " " + argss;
      //return name + " " + argss;
    }
    string make_var_name(string name) {
      return "<b>" + name + "</b>";
      //return name;
    }
    
    
    /////////////////////////
    //      HTMLing
    /////////////////////////
    string wrap_to_html(string s) {
      return "<!doctype html>\n<html>\n<head><meta charset=\"utf-8\"><title>main</title></head>\n<body>\n\n" + s + "\n</body></html>";
    }
    
    const string what_in_empty_B = "&#8226;";
    const string newline_html = "\n<br>";
    
    
    ////////////////////////
    //    CONCAT
    ////////////////////////
    
    string safe_concat(string a1, string a2, string delim) {
      if (a2.size() > 0) return a1 + delim + a2; else return a1;
    }
    
    ////   LISTS     /////
    string add_with_comma(string as, string a) {
      return as + ", " + a;
    }
    
    ////    BLOCKS    ////
    string concat_smth(string smth, string what) {
      return smth + " " + "<b style=\"font-family:monospace;color:Green;font-size:20px;\">(</b>" + what + "<b style=\"font-family:monospace;color:Green;font-size:20px;\">)</b>";
    }
    bool is_block(string s) {
      int i = 0;
      while (i < s.size() && s[i] == ' ') ++i;
      return (i < s.size() && s[i] == '{');
    }
    string attach_block(string ans, string body) { 
      if (is_block(body))
        return ans + " " + body;
      return ans + newline_html + body;
    }
    
    string concat_op(string l, string op, string r) {
      return l + " " + op + " " + r;
    }
    string concat_block(string l, string s, string r) {
      if (s == "") return l + r;
      return l + newline_html + s + make_command(r);
    }
    string concat_for(string e1, string e2, string e3) {
      string ans = "for (" + e1 + "; " + e2 + "; " + e3 + ")";
      //return attach_block(ans, body);
      return ans;
    }
    string concat_do(string body, string when) {
      return attach_block("do", body) + " " + "while (" + when + ")" + ";";
    }
    string concat_while(string when, string body) {
      return attach_block(concat_smth("while",when), body);
    }
    string concat_switch(string expr, string body) {
      return attach_block(concat_smth("switch", expr), body);
    }
    string concat_if(string expr, string body) {
      return attach_block(concat_smth("if", expr), body);
    }
    string concat_else(string cond, string ifbody, string elsebody) {
      string if_part = concat_if(cond, ifbody);
      if (is_block(ifbody)) {
        return attach_block(if_part + " else", elsebody);
      } else {
        return attach_block(if_part + newline_html + make_command("else"), elsebody);
      }
    }
    
    
    ///    BLOCK HELPERS    ///
    string concat_decl_to_blockinside(string x, string xs) {
      return x + newline_html + xs;
    }
    string concat_comm_to_blockinside(string x, string xs) {
      return x + newline_html + xs;
    }
    
    
    ///   ARRAYS    ////
    string concat_def_array(string name, string inside) {
      //return "<array_def:" + name + "[" + inside + "]>";
      return name + "<i style=\"font-family:serif;color:Navy;font-size:25px;\">[</i>" + inside + "<i style=\"font-family:serif;color:Navy;font-size:25px;\">]</i>";
    }
    string concat_call_array(string name, string inside) {
      //return "<array_call:" + name + "[" + inside + "]>";
      return name + "<i style=\"font-family:serif;color:Navy;font-size:20px;\">[</i>" + inside + "<i style=\"font-family:serif;color:Navy;font-size:20px;\">]</i>";
    }
    
    ///    COMMANDS   ////
    string make_case(string a1, string a2) {
      return "case " + a1 + ":" + newline_html + a2;
    }
    string make_label(string a1, string a2) {
      return "label: " + a1 + " : " + a2;
    }
    string make_default(string a1) {
      return "default:" + newline_html + a1;
    }
    string make_return(string a1) {
      return "return " + a1 + ";";
    }
    
    
    string concat_call_fun(string name, string inside) {
      return "<tt style=\"font-family:fantasy;color:Crimson;font-size:20px;\">" + name + "</tt>" +
        "<b style=\"font-family:serif;color:Navy;font-size:20px;\">(</b>" + "<u>" + inside + "</u>" + "<b style=\"font-family:serif;color:Navy;font-size:20px;\">)</b>";
      //return name + "(" + inside + ")";
    }
    
    
    int main();

%}

%token TYPE_SPEC TYPE
%token NUM STRING_S STRING_D CHAR 
%token IF ELSE CASE DEFAULT RETN MOVE SWITCH WHILE DO FOR
%token COLON SEMICOLON LB RB COMMA LQB RQB UNAR MUL ADD REL EQS AMP XOR OR EQ LFB RFB
%token ID DEFINED_TYPE OTHER BOOL COMMENT
%start all_file

%precedence "then"
%precedence ELSE

%%

lfb: LFB mb_comments {$$ = $1 + $2;}

__TYPE_SPEC: TYPE_SPEC mb_comments {$$ = $1 + $2;}

__SEMICOLON: SEMICOLON mb_comments {$$ = $1 + $2;}

mb_comments: {$$="";}|mb_comments COMMENT{$$ = $1 + "<b style=\"font-family:serif;color:Navy;font-size:20px;\">" + $2 + "</b>";}

__RFB: RFB mb_comments {$$ = $1 + $2;}









all_file: input { cout<<wrap_to_html($1); }

input:                  { $$ = ""; }
     | input input_item { $$ = $1 + $2; }
     | input error      { $$ = $1 + $2; }

    
////////////////////////
//        INPUT
////////////////////////
    
input_item: declaration{ $$=$1 + newline_html + newline_html; }


////////////////////////
//        EXPRS
////////////////////////
expr:        expr_equal                { $$ = $1; }

expr_equal:  expr_single EQ expr_equal { $$ = concat_op($1, $2, $3); } | expr_or          {$$ = $1;}

expr_or:     expr_or OR expr_xor       { $$ = concat_op($1, $2, $3); } | expr_xor         {$$ = $1;}

expr_xor:    expr_xor XOR expr_and     { $$ = concat_op($1, $2, $3); } | expr_and         {$$ = $1;}

expr_and:    expr_and AMP expr_eq      { $$ = concat_op($1, $2, $3); } | expr_eq          {$$ = $1;}

expr_eq:     expr_eq EQS expr_rel      { $$ = concat_op($1, $2, $3); } | expr_rel         {$$ = $1;}

expr_rel:    expr_rel REL expr_add     { $$ = concat_op($1, $2, $3); } | expr_add         {$$ = $1;}

expr_add:    expr_add ADD expr_mul     { $$ = concat_op($1, $2, $3); } | expr_mul         {$$ = $1;}

expr_mul:    expr_mul MUL expr_single  { $$ = concat_op($1, $2, $3); } | expr_single      {$$ = $1;}

expr_single: expr_post                 { $$ = $1; } | UNAR expr_single { $$ = $2; } | ADD expr_single { $$ = $1+$2; }

expr_post: expr_simple                 { $$ = $1; }
      | expr_post LQB expr RQB         { $$ = concat_call_array($1, $3); }
      | expr_post LB mb_arg_list RB    { $$ = concat_call_fun($1, $3); }  // fun call

mb_arg_list: arg_list { $$ = $1; } | { $$ = what_in_empty_B; }

arg_list: expr_equal {$$ = $1;}
        | arg_list COMMA expr_equal { $$ = add_with_comma($1, $3); }

expr_simple: ID     { $$ = $1; }
      | const       { $$ = $1; }
      | STRING_D    { $$ = make_string($1); }
      | LB expr RB  { $$ = $2; }
      
const: CHAR  { $$ = make_char($1); }
      | NUM  { $$ = make_decimal($1); }
      | BOOL { $$ = make_bool($1); }


////////////////////////
//     COMMANDS
////////////////////////
command_:command_labeled             { --depth; $$ = make_command($1); ++depth; }
      | goto                         { $$ = make_command($1); }
      | mb_expr __SEMICOLON            { $$ = make_command($1 + $2); }
      | {++depth;} command_if        { --depth; $$ = make_command($2); }
      | {++depth;} command_while     { --depth; $$ = make_command($2); }

command: command_ { $$=$1; } | block { $$ = make_command($1); }
      
block: {++depth;} lfb block_inside __RFB { --depth; $$ = concat_block($2, $3, $4); }

command_while: WHILE LB expr RB command_for_for         { $$ = concat_while($3, $5); }
      | DO { --depth; } block WHILE LB expr RB __SEMICOLON{ ++depth; $$ = concat_do($3, $6); }
      | forr command_                                   { $$ = attach_block($1, $2); }
      | forr {--depth;} block                           { ++depth; $$ = attach_block($1, $3); }

command_for_for: command_ { $$ = $1; }
      | { --depth; } block { ++depth; $$ = $2; }

forr: FOR LB mb_expr __SEMICOLON mb_expr __SEMICOLON mb_expr RB { $$ = concat_for($3,$5,$7); }

mb_expr: expr { $$ = $1; } | { $$ = ""; }

command_if: IF LB expr RB command_for_for { $$ = concat_if($3, $5); } %prec "then"
      | IF LB expr RB command_for_for ELSE command_for_for { --depth; $$ = concat_else($3, $5, $7); ++depth;}
      | {--depth; } SWITCH LB expr RB block           { ++depth; $$ = concat_switch($4, $6); }

command_labeled: ID COLON command             { $$ = make_label($1, $3); }
      | CASE expr_or COLON command { $$ = make_case($2, $4); }
      | DEFAULT COLON command      { $$ = make_default($3); }

block_inside: declaration block_inside { $$ = concat_decl_to_blockinside($1, $2); }
      | command block_inside           { $$ = concat_comm_to_blockinside($1, $2); }
      |                                { $$ = ""; }

goto: MOVE __SEMICOLON { $$ = $1 + ";"; }
      | RETN mb_expr __SEMICOLON { $$ = make_return($2); }


////////////////////////
//   DECLARATIONS
////////////////////////
declaration: decl_spec mbType_list mb_block { $$ = make_command(make_fun_type($1, $2, $3)); }

mb_block: __SEMICOLON {$$= $1;} | block { $$=$1; }

decl_spec: __TYPE_SPEC mb_decl_spec{ $$ = safe_concat($1, $2, " "); }
        |  TYPE mb_decl_spec     { $$ = safe_concat($1, $2, " "); }

mb_decl_spec: decl_spec          { $$ = $1; } | { $$ = ""; }

mbType_list: type_list           { $$ = $1; } | { $$ = ""; }

type_list: decl_init             { $$ = $1; }
      | type_list COMMA decl_init{ $$ = add_with_comma($1, $3); }   // int q,w=0,e,r=5;

decl_init: decl mb_initializer   { $$ = safe_concat($1, $2, " "); }

mb_initializer: EQ initializer   { $$ = $1 + " " + $2; } | { $$ = ""; }

initializer: expr_equal          { $$ = $1; }
      | lfb initializer_list __RFB { $$ = make_initializer_list($2); }
      
initializer_list: initializer {$$ = $1;}
        | initializer_list COMMA initializer { $$ = add_with_comma($1, $3); }

decl: ID                         { $$ = make_var_name($1); }
      | LB decl RB               { $$ = $2; }
      | decl LQB mb_expr_or RQB  { $$ = concat_def_array($1, $3); }
      | decl LB param_list RB    { $$ = make_fun_decl($1, $3); } // $1 = name of function; $3 = param_list

param_list: param_decl { $$ = $1; }
        | param_list COMMA param_decl { $$ = add_with_comma($1, $3); } | { $$ = what_in_empty_B; } // int a(int q, int w, int e)
        
param_decl: decl_spec decl_init { $$ = make_param_decl($1, $2); }

mb_expr_or: expr_or { $$ = $1; } | { $$ = ""; }

%%

int main()
{//bison -d parser.y --verbose && flex lexer.lex && g++ -std=c++11 -c lex.yy.c parser.tab.c && g++ -o parser lex.yy.o parser.tab.o -ll -ly && ./parser < test.cpp > out.html
    return yyparse();
}