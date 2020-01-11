#pragma once

#include <iostream>
#include <cstdlib>
#include <string>
#include <cstdio>
#include "compiler.hpp"

using namespace std;

// linked list
struct symrec
{
    string name;
    long int lenght;
    long int startIndex;
    bool isInit;
    long int storedAt;
    struct symrec *next;
};

typedef struct symrec symrec;
symrec* putsym();
symrec* getsym(string name);
void init_var(string name, int lineno);
void init_array(string name, long int startIndex, long int endIndex, int lineno);
bool symbol_exists(string name);
void printTable();