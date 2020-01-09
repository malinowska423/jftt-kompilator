%{
#include "compiler.hpp"

extern int yylex();
extern int yylineno;
extern FILE *yyin;
int yyerror(const string str);
%}

// %define parse.error verbose
// %expect 0

%union {
    std::string *pidentifier;
    long long int num;
}

//Tokens
%start program
%token DECLARE _BEGIN END
%token IF WHILE DO
%token FOR 
%token THEN ELSE ENDIF FROM TO DOWNTO ENDFOR ENDWHILE ENDDO
%token READ WRITE       
%token LE GE LEQ GEQ EQ NEQ
%token ASSIGN
%token ERROR
%token <pidentifier> pidentifier
%token <num> num
//Types
// %type <var> value
// %type <id> identifier
// %type <cond> condition;

//Operators precedence
%left PLUS MINUS
%left TIMES DIV MOD
     

%%
program:

    DECLARE declarations _BEGIN commands END                            {shout(1);}
    | _BEGIN commands END                                               {}
    ;

declarations:

    declarations',' pidentifier                                     {shout(2);}
    | declarations',' pidentifier'('num':'num')'                      {shout(3);}
    | pidentifier                                                   {}
    | pidentifier'('num':'num')'                                    {shout(5);}
    ;

commands:

    commands command                                                    {shout(6); }
    | command                                                           {shout(7); }
    ;

command:

    identifier ASSIGN expression';'                                   {shout(8);}
    | IF condition THEN commands ELSE commands ENDIF                  {}
    | IF condition THEN commands ENDIF                                {}
    | WHILE condition DO commands ENDWHILE                            {}
    | DO commands WHILE condition ENDDO                               {}
    | FOR pidentifier FROM value TO value DO commands ENDFOR          {}
    | FOR pidentifier FROM value DOWNTO value DO commands ENDFOR      {}
    | READ identifier';'                                              {}
    | WRITE value';'                                                  {}
    ;

expression:

    value                       {}
    | value PLUS value          {}
    | value MINUS value         {}
    | value TIMES value         {}
    | value DIV value           {}
    | value MOD value           {}
    ;

condition:

    value EQ value              {}
    | value NEQ value           {}
    | value LE value            {}
    | value GE value            {}
    | value LEQ value           {}
    | value GEQ value           {}
    ;

value:

    num                         {shout(9);}
    | identifier                {shout(10);}
    ;

identifier:

    pidentifier                                 {shout(11);}
    | pidentifier'('pidentifier')'              {shout(12);}
    | pidentifier'('num')'                      {shout(13);}
    ;

%%


int main(int argv, char* argc[]) {
    if( argv != 2 ) {
        cerr << "Prawidlowe wywolanie: kompilator plik_wejsciowy plik_wyjsciowy" << endl;
        return 1;
    }

    yyin = fopen(argc[1], "r");
    if (yyin == NULL){
        cout << "Plik nie istnieje" << endl;
        return 1;
    }

	yyparse();

    cout << "Kompilacja zakonczona pomyslnie" << endl;
	return 0;
}

int yyerror(string err) {
    cout << "Wystapil blad (linia " << yylineno << "):\t" << err << endl;
    exit(1);
}