#include "symbol-table.hpp"

symrec *sym_table = (symrec *)0;
long long int offset = 1;

symrec *putsym(string sym_name, long long int lenght, long long int startIndex, sym_type type)
{
    symrec *ptr;
    ptr = new symrec;
    ptr->name = sym_name;
    ptr->lenght = lenght;
    ptr->startIndex = startIndex;
    ptr->isInit = false;
    ptr->storedAt = offset;
    offset += lenght;
    ptr->type = type;
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
        s = putsym(name, 1, 1, VARIABLE);
    }
    else
    {
        error("zmienna " + name + " juz istnieje", lineno);
    }
}

void init_array(string name, long long int startIndex, long long int endIndex, int lineno)
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
        s = putsym(name, (endIndex - (startIndex - 1)), startIndex, ARRAY);
    }
}

bool symbol_exists(string name)
{
    symrec *s;
    s = getsym(name);
    if (s == 0)
    {
        return false;
    }
    else
    {
        return true;
    }
}

long long int get_offset()
{
    return offset;
}