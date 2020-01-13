%option noyywrap
%option yylineno
%option nounput
     
%{
#include <iostream>
#include <string>
#include <vector>
#include "parser.h"
%}

IDENTIFIER              [_a-z]
DIGIT                   [-]?([1-9][0-9]*|0)
PUNCT                   [\(\)\:\;,]

%x COMMENT

%%
"DECLARE"               { return DECLARE; }
"BEGIN"                 { return _BEGIN; }
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

"PLUS"                  { return PLUS; }
"MINUS"                 { return MINUS; }
"TIMES"                 { return TIMES; }
"DIV"                   { return DIV; }
"MOD"                   { return MOD; }
{IDENTIFIER}+           { 
                            yylval.pidentifier = new std::string(yytext);
                            return pidentifier;
                        }
{DIGIT}+                { yylval.num = std::stoll(yytext); return num; }
{PUNCT}                 { return yytext[0]; }
\[.*\]                  ;
"["                     { BEGIN(COMMENT); }
<COMMENT>{
    [^\]]               ;
    "]"                 BEGIN(INITIAL);
}

[ \t\r\n]+              ;
.                       { return ERROR; }
%%