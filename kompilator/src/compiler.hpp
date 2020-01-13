#pragma once

#include <string.h>
#include <iostream>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <climits>
#include <memory>
#include <math.h>
#include <vector>
#include <stack>
#include <map>

using namespace std;

enum var_type
{
    VAL,
    VAR,
    PTR
};

struct variable
{
    string name;
    long long int index;
    string indexName;
    var_type type;
};
typedef struct variable var;

enum cond_type
{
    JEQ,
    JNEQ,
    JGE,
    JGEQ
};

typedef vector<string> vecS;
struct condition
{
    long long int index;
    cond_type type;
    vecS commands;
};
typedef struct condition cond;

void flush_to_file(vecS);
void set_output_filename(char *);
void open_file();
void close_file();
void shout(int);

void error(string msg, int lineno);
long int get_errors();
long int get_loaded_index();

vecS *pass_cmd(vecS *);
vecS *pass_cmd(vecS *, vecS *);

vecS *cmd_assign(var *variable, var *expr, int lineno);
vecS *cmd_if(cond *, vecS *, int);
vecS *cmd_read(var *current, int lineno);
vecS *cmd_write(var *current, int lineno);

var *cmd_num(long long int value, int lineno);
var *cmd_pid(string name, long long int index, int lineno);
var *cmd_pid_arr(string name, string indexName, int lineno);
void cmd_end();

var *expr_val(var *value, int lineno);
var *expr_plus(var *a, var *b, int lineno);
var *expr_minus(var *a, var *b, int lineno);
var *plus_minus(var *a, var *b, int lineno, string command);

cond *cond_eq(var *, var *, int);
cond *cond_neq(var *, var *, int);
cond *cond_ge(var *, var *, int);
cond *cond_geq(var *, var *, int);
cond *set_condition(var *, var *, int, cond_type);

void assign_to_p0(long long int value);
var *set_temp_var(var *);
var *set_temp_ptr(var *);
long long get_var_index(var *);
string dec_to_bin(long long int);
void check_jumps(vecS*);
