#include "compiler.hpp"
#include "symbol-table.hpp"

long int errors = 0;
long int loadedIndex = 0;
vector<string> commands;
var *current_var;

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

void cmd_read(int lineno)
{
    long int i = getsym(current_var->name)->storedAt + current_var->index;
    commands.push_back("GET\n");
    loadedIndex = i;
    commands.push_back("STORE " + to_string(i) + "\n");
}

void cmd_write(int lineno)
{
    long int i = getsym(current_var->name)->storedAt + current_var->index;
    if (loadedIndex != i)
    {
        commands.push_back("LOAD " + to_string(i) + "\n");
        loadedIndex = i;
    }
    commands.push_back("PUT\n");
}

void cmd_pid(string name, long int index, int lineno)
{
    if (symbol_exists(name))
    {
        if (current_var == nullptr)
        {
            current_var = (var *)malloc(sizeof(var));
        }
        if (current_var->name.compare(name) != 0) //different var
        {
            current_var->name = name;
            current_var->index = index;
        }
        else if (current_var->index != index) //same var, different index
        {
            current_var->index = index;
        } //same var - do nothing
    }
    else
    {
        error("zmienna " + name + " nie zostala zainicjalizowana", lineno);
    }
}

void cmd_end()
{
    commands.push_back("HALT");
}