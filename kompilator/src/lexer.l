%option noyywrap
%option yylineno
     
%{
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <cstring>
#include "parser.tab.h"

const std::string errorMsg = "error: ";

%}

IDENTIFIER              [_a-z]
DIGIT                   [0-9]
PUNCT                   [\(\)\:\;]

%x COMMENT

%%
"DECLARE"               { return DECLARE; }
"BEGIN"                 { return BEGIN; }
"END"                   { return END; }

"READ"                  { return READ; }
"WRITE"                 { return WRITE; }

"IF"                    { return IF; }
"THEN"                  { return THEN; }
"ELSE"                  { return ELSE; }
"ENDIF"                 { return ENDIF; }

"WHILE"                 { return WHILE; }
"DO"                    { return DO; }
"ENDWHILE"              { return ENDWHILE; }
"ENDDO"                 { return ENDDO; }

"FOR"                   { return FOR; }
"FROM"                  { return FROM; }
"TO"                    { return TO; }
"DOWNTO"                { return DOWNTO; }
"ENDFOR"                { return ENDFOR; }

"ASSIGN"                { return ASSIGN; }

"EQ"                    { return EQ; }
"NEQ"                   { return NEQ; }
"LE"                    { return LE; }
"GE"                    { return GE; }
"LEQ"                   { return LEQ; }
"GEQ"                   { return GEQ; }

"PLUS"                  { return ADD; }
"MINUS"                 { return SUB; }
"TIMES"                 { return TIMES; }
"DIV"                   { return DIV; }
"MOD"                   { return MOD; }
{IDENTIFIER}+           { 
                            yylval.pidentifier = new std::string(yytext);
                            return pidentifier;
                        }
{DIGIT}+                { yylval.num = std::stoll(yytext); return num; }
{PUNCT}                 { return yytext[0]; }
\[.*\]                  { /*ignore comments TODO: make comments better again*/ }
"["                     { BEGIN(COMMENT); }
<COMMENT>{
    [^\]]               ;
    "]"                 BEGIN(INITIAL);
}

[ \t\r\n]+                { /*ignore whitespaces*/ }
.                       { return ERROR; }
%%