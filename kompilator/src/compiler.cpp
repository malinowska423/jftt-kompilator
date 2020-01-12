#include "compiler.hpp"
#include "symbol-table.hpp"

long int errors = 0;
vector<string> commands;
vector<var *> temp;
// var *current_var;

void shout(int num)
{
    cout << "Im workin " << num << endl;
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

void cmd_assign(var *variable, var *expr, int lineno)
{
    long long int i;
    switch (variable->type)
    {
    case VAR:
    {
        if (expr->type == PTR)
        {
            i = set_temp_ptr(expr)->index;
            commands.push_back("LOADI " + to_string(i));
        }
        else
        {
            if (expr->index != 0)
            {
                i = expr->index;
                commands.push_back("LOAD " + to_string(i));
            }
            i = get_var_index(variable);
        }
        commands.push_back("STORE " + to_string(i));
    }
    break;
    case PTR:
    {
        if (expr->index == 0)
        {
            expr = set_temp_var(expr);
        }
        var *temp;
        temp = set_temp_ptr(variable);
        if (expr->type == PTR)
        {
            i = set_temp_ptr(expr)->index;
            commands.push_back("LOADI " + to_string(i));
        }
        else
        {
            i = expr->index;
            commands.push_back("LOAD " + to_string(i));
        }
        i = temp->index;
        commands.push_back("STOREI " + to_string(i));
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
        commands.push_back("GET");
        commands.push_back("STORE " + to_string(i));
    }
    else if (current->type == PTR)
    {
        long long i = set_temp_ptr(current)->index;
        commands.push_back("GET");
        commands.push_back("STOREI " + to_string(i));
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
    {
        long long int i = current->index;
        commands.push_back("LOAD " + to_string(i));
    }
    break;
    case VAR:
    {
        long long int i = get_var_index(current);
        commands.push_back("LOAD " + to_string(i));
    }
    break;
    case PTR:
    {
        var *temp = set_temp_ptr(current);
        long long int i = temp->index;
        commands.push_back("LOADI " + to_string(i));
    }
    break;
    default:
        error("nieprawidlowa zmienna", lineno);
        break;
    }
    commands.push_back("PUT");
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
    commands.push_back("SUB 0");
    for (long long i = 0; i < value; i++)
    {
        commands.push_back("INC");
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
    commands.push_back("STORE " + to_string(i));
    return variable;
}

var *set_temp_ptr(var *current)
{
    assign_to_p0(current->index);
    symrec *s;
    s = getsym(current->indexName);
    commands.push_back("ADD " + to_string(s->storedAt));
    return set_temp_var(nullptr);
}

long long int get_var_index(var *current)
{
    return getsym(current->name)->storedAt + current->index - getsym(current->name)->startIndex;
}

void print_to_file(char *out)
{
    ofstream file;
    file.open(out);
    for (int cmd = 0; cmd < commands.size(); cmd++)
    {
        file << commands.at(cmd) << endl;
    }
    file.close();
}
