%{
#include "compiler.hpp"
#include "symbol-table.hpp"

extern int yylex();
extern int yylineno;
extern FILE *yyin;
int yyerror(const string str);

%}

%union {
    std::string *pidentifier;
    long long int num;
    struct variable *variable;
    struct condition *cond;
    std::vector<std::string> *command;
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

%type <variable> value
%type <variable> identifier
%type <variable> expression
%type <cond> condition
%type <command> command
%type <command> commands

%left PLUS MINUS
%left TIMES DIV MOD
     

%%
program:

    DECLARE declarations _BEGIN commands END                         {cmd_end($4);}
    | _BEGIN commands END                                            {cmd_end($2);}
    ;

declarations:

    declarations',' pidentifier                                     {init_var(*$3, yylineno);}
    | declarations',' pidentifier'('num':'num')'                    {init_array(*$3, $5, $7, yylineno);}
    | pidentifier                                                   {init_var(*$1, yylineno);}
    | pidentifier'('num':'num')'                                    {init_array(*$1, $3, $5, yylineno);}
    ;

commands:

    commands command                                                    {$$ = pass_cmd($1, $2);}
    | command                                                           {$$ = pass_cmd($1);}
    ;

command:

    identifier ASSIGN expression';'                                   {$$ = cmd_assign($1, $3, yylineno);}
    | IF condition THEN commands ELSE commands ENDIF                  {$$ = cmd_if_else($2, $4, $6, yylineno);}
    | IF condition THEN commands ENDIF                                {$$ = cmd_if($2, $4, yylineno);}
    | WHILE condition DO commands ENDWHILE                            {$$ = cmd_while($2, $4, yylineno);}
    | DO commands WHILE condition ENDDO                               {$$ = cmd_do_while($4, $2, yylineno);}
    | FOR pidentifier FROM value TO value DO commands ENDFOR          {$$ = cmd_for(*$2, $4, $6, $8, yylineno);}
    | FOR pidentifier FROM value DOWNTO value DO commands ENDFOR      {$$ = cmd_for_downto(*$2, $4, $6, $8, yylineno);}
    | READ identifier';'                                              {$$ = cmd_read($2, yylineno);}
    | WRITE value';'                                                  {$$ = cmd_write($2, yylineno);}
    ;

expression:

    value                       {$$ = expr_val($1, yylineno);}
    | value PLUS value          {$$ = expr_plus($1, $3, yylineno);}
    | value MINUS value         {$$ = expr_minus($1, $3, yylineno);}
    | value TIMES value         {$$ = expr_times($1, $3, yylineno);}
    | value DIV value           {$$ = expr_div($1, $3, yylineno);}
    | value MOD value           {$$ = expr_mod($1, $3, yylineno);}
    ;

condition:

    value EQ value              {$$ = cond_eq($1, $3, yylineno);}
    | value NEQ value           {$$ = cond_neq($1, $3, yylineno);}
    | value LE value            {$$ = cond_ge($3, $1, yylineno);}
    | value GE value            {$$ = cond_ge($1, $3, yylineno);}
    | value LEQ value           {$$ = cond_geq($3, $1, yylineno);}
    | value GEQ value           {$$ = cond_geq($1, $3, yylineno);}
    ;

value:

    num                         {$$ = cmd_num($1, yylineno);}
    | identifier                {$$ = $1;}
    ;

identifier:

    pidentifier                                 {$$ = cmd_pid(*$1, 1, yylineno);}
    | pidentifier'('pidentifier')'              {$$ = cmd_pid_arr(*$1, *$3, yylineno);}
    | pidentifier'('num')'                      {$$ = cmd_pid(*$1, $3, yylineno);}
    ;

%%


int main(int argv, char* argc[]) {
    if( argv != 3 ) {
        cerr << "Prawidlowe wywolanie: ./kompilator plik_wejsciowy plik_wyjsciowy" << endl;
        return 1;
    }

    yyin = fopen(argc[1], "r");
    if (yyin == NULL){
        cout << "Plik nie istnieje" << endl;
        return 1;
    }
    set_output_filename(argc[2]);
    open_file();

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
    if (err == "syntax error") {
        err = "niepoprawna skladnia";
    }
    cerr << "Blad (linia " << yylineno << "):\t" << err << endl;
    cout << "Kompilacja nieudana" << endl;
    exit(1);
}