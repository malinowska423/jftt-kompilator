#include "compiler.hpp"
#include "symbol-table.hpp"

long int errors = 0;
long int loadedIndex = 0;
vector<string> commands;
vector<var *> temp;
// var *current_var;

var *set_temp_ptr(var *current)
{
    assign_to_p0(current->index);
    symrec *s;
    s = getsym(current->indexName);
    commands.push_back("ADD " + to_string(s->storedAt) + "\n");
    return set_temp_var(nullptr);
}

void shout(int num)
{
    cout << "Im workin " << num << endl;
}

void printCommands()
{
    cout << "Komendy: \n";
    for (int i = 0; i < commands.size(); i++)
    {
        cout << commands.at(i);
    }
    cout << endl;
}

void error(string msg, int lineno)
{
    if (errors == 0)
    {
        cerr << "Bledy:\n";
    }
    errors++;
    cerr << "\tlinia " << lineno << ": " << msg << endl;
    exit(1);
}

long int get_errors()
{
    return errors;
}

long int get_loaded_index()
{
    return loadedIndex;
}

void cmd_assign(var *variable, var *expr, int lineno)
{
    cout << variable->name << " assign " << expr->type << endl;
    long long int i;
    switch (variable->type)
    {
    case VAR:
    {
        i = getsym(variable->name)->storedAt + variable->index - getsym(variable->name)->startIndex;
        commands.push_back("STORE " + to_string(i) + "\n");
    }
    break;
    case PTR:
    {
        assign_to_p0(variable->index);
        loadedIndex = -1;
        symrec *s;
        s = getsym(variable->indexName);
        commands.push_back("ADD " + to_string(s->storedAt) + "\n");
        var *temp = set_temp_var(nullptr);
        if (expr->type == PTR)
        {
            long long i = set_temp_ptr(expr)->index;
            commands.push_back("LOADI " + to_string(i) + "\n");
        }
        else
        {
            long long i = expr->index;
            commands.push_back("LOAD " + to_string(i) + "\n");
        }
        long long i = temp->index;
        commands.push_back("STOREI " + to_string(i) + "\n");
    }
    break;
    default:
        error("nieprawidlowa zmienna", lineno);
        break;
    }
}

void cmd_read(var *current, int lineno)
{
    if (current->type == VAR)
    {
        long long int i = getsym(current->name)->storedAt + current->index - getsym(current->name)->startIndex;
        commands.push_back("GET\n");
        loadedIndex = i;
        commands.push_back("STORE " + to_string(i) + "\n");
    }
    else if (current->type == PTR)
    {
        long long i = set_temp_ptr(current)->index;
        commands.push_back("GET\n");
        commands.push_back("STOREI " + to_string(i) + "\n");
    }
    else
    {
        error("bledna zmienna", lineno);
    }
}

void cmd_write(var *current, int lineno)
{
    switch (current->type)
    {
    case VAL:
        assign_to_p0(current->index);
        break;
    case VAR:
    {
        long int i = getsym(current->name)->storedAt + current->index - getsym(current->name)->startIndex;
        if (loadedIndex != i)
        {
            commands.push_back("LOAD " + to_string(i) + "\n");
            loadedIndex = i;
        }
    }
    break;
    case PTR:
    {
        assign_to_p0(current->index);
        loadedIndex = -1;
        symrec *s;
        s = getsym(current->indexName);
        commands.push_back("ADD " + to_string(s->storedAt) + "\n");
        commands.push_back("LOADI 0\n");
    }
    break;
    default:
        error("nieprawidlowa zmienna", lineno);
        break;
    }
    commands.push_back("PUT\n");
}

var *cmd_num(long long int value, int lineno)
{
    var *current_var;
    current_var = (var *)malloc(sizeof(var));
    current_var->index = value;
    current_var->type = VAL;
    return current_var;
}

var *cmd_pid(string name, long long int index, int lineno)
{
    symrec *s;
    s = getsym(name);
    if (s == 0)
    {
        error("zmienna " + name + " nie zostala zainicjalizowana", lineno);
        return nullptr;
    }
    else if (index > 1 && s->type != ARRAY)
    {
        error("zmienna " + name + " nie jest zmienna tablicowa", lineno);
        return nullptr;
    }
    else if (s->type == ARRAY && (index < s->startIndex || index >= (s->startIndex + s->lenght)))
    {
        error("indeks " + to_string(index) + " jest poza zakresem tablicy " + name, lineno);
        return nullptr;
    }
    else
    {
        var *current_var;
        current_var = (var *)malloc(sizeof(var));
        current_var->name = name;
        current_var->index = index;
        current_var->type = VAR;
        return current_var;
    }
}

var *cmd_pid_arr(string name, string indexName, int lineno)
{
    symrec *s;
    s = getsym(name);
    if (s == 0)
    {
        error("zmienna " + name + " nie zostala zainicjalizowana", lineno);
        return nullptr;
    }
    else if (!symbol_exists(indexName))
    {
        error("zmienna " + indexName + " nie zostala zainicjalizowana", lineno);
    }
    else
    {
        var *current_var;
        current_var = (var *)malloc(sizeof(var));
        current_var->name = name;
        current_var->index = s->storedAt - s->startIndex;
        current_var->indexName = indexName;
        current_var->type = PTR;
        return current_var;
    }
}

void cmd_end()
{
    commands.push_back("HALT");
}

var *expr_val(var *value, int lineno)
{
    switch (value->type)
    {
    case VAL:
    {
        assign_to_p0(value->index);
        value->index = 0;
    }
    break;
    case VAR:
    {
        long long int i = get_var_index(value);
        value->index = i;
    }
    break;
    case PTR:
    {

    }
    break;
    default:
        error("nieprawidlowe wyrazenie", lineno);
        break;
    }
    return value;
}

var *expr_plus(var *a, var *b, int lineno)
{
}

void assign_to_p0(long long int value)
{
    commands.push_back("SUB 0\n");
    for (long long i = 0; i < value; i++)
    {
        commands.push_back("INC\n");
    }
}

var *set_temp_var(var *variable)
{
    if (variable == nullptr)
    {
        variable = (var *)malloc(sizeof(var));
        variable->type = VAL;
    }

    long long int i = get_offset() + temp.size();
    variable->index = i;
    temp.push_back(variable);
    commands.push_back("STORE " + to_string(i) + "\n");
    return variable;
}

long long int get_var_index(var *current)
{
    return getsym(current->name)->storedAt + current->index - getsym(current->name)->startIndex;
}

