%{
#include "compiler.hpp"
#include "symbol-table.hpp"

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
    struct variable * variable;
}


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
%type <variable> value
%type <variable> identifier
%type <variable> expression
// %type <cond> condition;

//Operators precedence
%left PLUS MINUS
%left TIMES DIV MOD
     

%%
program:

    DECLARE declarations            {open_file();}
    _BEGIN commands                 {}
    END                            {cmd_end();close_file();}
    | _BEGIN commands END                                               {}
    ;

declarations:

    declarations',' pidentifier                                     {init_var(*$3, yylineno);}
    | declarations',' pidentifier'('num':'num')'                    {init_array(*$3, $5, $7, yylineno);}
    | pidentifier                                                   {init_var(*$1, yylineno);}
    | pidentifier'('num':'num')'                                    {init_array(*$1, $3, $5, yylineno);}
    ;

commands:

    commands command                                                    {flush_to_file();}
    | command                                                           {flush_to_file();}
    ;

command:

    identifier ASSIGN expression';'                                   {cmd_assign($1, $3, yylineno);}
    | IF condition THEN commands ELSE commands ENDIF                  {}
    | IF condition THEN commands ENDIF                                {}
    | WHILE condition DO commands ENDWHILE                            {}
    | DO commands WHILE condition ENDDO                               {}
    | FOR pidentifier FROM value TO value DO commands ENDFOR          {}
    | FOR pidentifier FROM value DOWNTO value DO commands ENDFOR      {}
    | READ identifier';'                                              {cmd_read($2, yylineno);}
    | WRITE value';'                                                  {cmd_write($2, yylineno);}
    ;

expression:

    value                       {$$ = expr_val($1, yylineno);}
    | value PLUS value          {$$ = expr_plus($1, $3, yylineno);}
    | value MINUS value         {$$ = expr_minus($1, $3, yylineno);}
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

    num                         {$$ = cmd_num($1, yylineno);}
    | identifier                {$$ = $1;}
    ;

identifier:

    pidentifier                                 {$$ = cmd_pid(*$1,1, yylineno);}
    | pidentifier'('pidentifier')'              {$$ = cmd_pid_arr(*$1, *$3, yylineno);}
    | pidentifier'('num')'                      {$$ = cmd_pid(*$1, $3, yylineno);}
    ;

%%


int main(int argv, char* argc[]) {
    if( argv != 3 ) {
        cerr << "Prawidlowe wywolanie: kompilator plik_wejsciowy plik_wyjsciowy" << endl;
        return 1;
    }

    yyin = fopen(argc[1], "r");
    if (yyin == NULL){
        cout << "Plik nie istnieje" << endl;
        return 1;
    }
    set_output_filename(argc[2]);

	yyparse();

    if (get_errors() == 0) {
        cout << "Kompilacja zakonczona pomyslnie" << endl;
        return 0;
    } else {
        cout << "Kompilacja nieudana" << endl;
        return 1;
    }
}

int yyerror(string err) {
    cerr << "Wystapil blad (linia " << yylineno << "):\t" << err << endl;
    cout << "Kompilacja nieudana" << endl;
    exit(1);
}