%{
#include "compiler.hpp"

extern int yylex();
extern int yylineno;
extern FILE *yyin;
int yyerror(const string str);
%}

%define parse.error verbose
%expect 0

%union {
    std::string *pidentifier;
    long long int num;
}

//Tokens
%start program
%token DECLARE _BEGIN END
%token <lbl> IF WHILE DO
%token <lfor> FOR 
%token THEN ELSE ENDIF FROM TO DOWNTO ENDFOR ENDWHILE ENDDO
%token READ WRITE       
%token LE GE LEQ GEQ EQ NEQ
%token ASSIGN
%token ERROR
%token <pidentifier> pidentifier
%token <num> num
//Types
%type <var> value
%type <id> identifier
%type <cond> condition;

//Operators precedence
%left PLUS MINUS
%left TIMES DIV MOD
     

%%
program:

    DECLARE declarations _BEGIN commands END                            {}
    | _BEGIN commands END                                               {}
    ;

declarations:

    declarations',' pidentifier                                     {}
    | declarations',' pidentifier'('num':'num')'                      {}
    | pidentifier                                                   {}
    | pidentifier'('num':'num')'                                    {}
    ;

commands:

    commands command                                                    { }
    | command                                                           { }
    ;

command:

    identifier ASSIGN expression';'                                   {}
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

    num                         {}
    | identifier                {}
    ;

identifier:

    pidentifier                                 {}
    | pidentifier'('pidentifier')'              {}
    | pidentifier'('num')'                      {}
    ;

%%


int main(int argv, char* argc[]) {
    if( argv != 3 ) {
        cerr << "Prawidlowe wywolanie: kompilator plik_wejsciowy plik_wyjsciowy" << endl;
        return 1;
    }

    string msg =". .. ... ";
    for (const char c: msg) {
        cout << c << flush;
        usleep(80000);
    }

    // yyin = fopen(argc[1], "r");
    // if (yyin == NULL)
    //     error(argc[1], 0, "File does not exist:");
    // createRegisters();
	yyparse();
    // optymize(argc[0]);
    // print(argc[2]);
    cout << "Compiled without errors •ᴗ•\n" << endl;
	return 0;
}

int yyerror(string err) {
    // cout << "(╯°□°）╯︵ ┻━┻\n\n";
    // usleep(500000);
    // cout << "\e[1m\x1B[31m[ ERROR ]\e[0m \e[1m[ LINE " << yylineno << " ] \e[1m\x1B[31m" << err << ".\e[0m\n" << endl;
    cout << "Error" << endl;
    exit(1);
}