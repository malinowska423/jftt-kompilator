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

void shout(int num);
void printCommands();

void error(string msg, int lineno);
long int get_errors();
long int get_loaded_index();


void cmd_assign(var* variable, var* expr, int lineno);
void cmd_read(var* current, int lineno);
void cmd_write(var* current, int lineno);
var *cmd_num(long long int value, int lineno);
var *cmd_pid(string name, long long int index, int lineno);
var *cmd_pid_arr(string name, string indexName, int lineno);
void cmd_end();

void assign_to_p0(long long int value);