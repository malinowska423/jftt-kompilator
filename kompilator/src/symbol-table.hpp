#pragma once

#include <iostream>
#include <cstdlib>
#include <string>
#include <cstdio>
#include "compiler.hpp"

using namespace std;

enum sym_type
{
    VARIABLE,
    ARRAY
};

struct symrec
{
    string name;
    long long int lenght;
    long long int startIndex;
    bool isInit;
    long long int storedAt;
    sym_type type;
    struct symrec *next;
};

typedef struct symrec symrec;
symrec *putsym();
symrec *getsym(string);
void init_var(string, int);
void init_array(string, long long int, long long int, int);
bool symbol_exists(string);
long long int get_offset();