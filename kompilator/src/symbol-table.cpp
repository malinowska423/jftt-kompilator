#include "symbol-table.hpp"

symrec *sym_table = (symrec *)0;
long int offset = 1;

symrec *putsym(string sym_name, long int lenght, long int startIndex)
{
    symrec *ptr;
    ptr = (symrec *)malloc(sizeof(symrec));
    // ptr->name = (char *)malloc(strlen(sym_name) + 1);
    // strcpy(ptr->name, sym_name);
    ptr->name = sym_name;
    ptr->lenght = lenght;
    ptr->startIndex = startIndex;
    ptr->isInit = false;
    ptr->storedAt = offset;
    offset += lenght; 
    ptr->next = (struct symrec *)sym_table;
    sym_table = ptr;
    return ptr;
}

symrec *getsym(string sym_name)
{
    symrec *ptr;
    for (ptr = sym_table; ptr != (symrec *)0; ptr = (symrec *)ptr->next)
    {
        if (ptr->name.compare(sym_name) == 0)
        {
            return ptr;
        }
    }
    return 0;
}


void init_var(string name, int lineno)
{
    symrec *s;
    s = getsym(name);
    if (s == 0)
    {
        s = putsym(name, 1, 1);
    }
    else
    {
        error("zmienna " + name + " juz istnieje", lineno);
    }
}

void init_array(string name, long int startIndex, long int endIndex, int lineno)
{
    symrec *s;
    s = getsym(name);
    if (s != 0)
    {
        error("zmienna " + name + " juz istnieje", lineno);
    }
    else if (startIndex > endIndex)
    {
        error("nieprawidlowy zakres tablicy", lineno);
    }
    else
    {
        s = putsym(name, (endIndex - (startIndex - 1)), startIndex);
    }
}

bool symbol_exists(string name) {
    symrec *s;
    s = getsym(name);
    if (s == 0) {
        return false;
    } else {
        return true;
    }
}

void printTable()
{
    symrec *ptr;
    for (ptr = sym_table; ptr != (symrec *)0; ptr = (symrec *)ptr->next)
    {
        cout << ptr->name << "(" << ptr->storedAt << ")\t";
    }
    cout << endl;
}