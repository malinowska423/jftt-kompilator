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

struct variable
{
    string name;
    long int index;
};
typedef struct variable var;

void shout(int num);
void printCommands();

void error(string msg, int lineno);
long int get_errors();
long int get_loaded_index();

void cmd_read(int lineno);
void cmd_write(int lineno);
void cmd_pid(string name, long int index, int lineno);
void cmd_end();