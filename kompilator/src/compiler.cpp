#include "compiler.hpp"
#include "symbol-table.hpp"

long int errors = 0;
vector<string> commands;
vector<var *> temp;
var *const_one = nullptr;
char *output_filename;
ofstream _file;

void shout(int nmbr)
{
    cout << "Hi " << nmbr << endl;
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

vecS *pass_cmd(vecS *_commands)
{
    return _commands;
}

vecS *pass_cmd(vecS *_commands, vecS *_command)
{
    _commands->insert(_commands->end(), _command->begin(), _command->end());
    _command->clear();
    return _commands;
}

vecS *cmd_assign(var *variable, var *expr, int lineno)
{
    vecS *_commands = new vecS();
    _commands->insert(_commands->end(), commands.begin(), commands.end());
    commands.clear();
    long long int i;
    switch (variable->type)
    {
    case VAR:
    {
        if (expr->type == PTR)
        {
            i = set_temp_ptr(expr)->index;
            _commands->insert(_commands->end(), commands.begin(), commands.end());
            commands.clear();
            _commands->push_back("LOADI " + to_string(i));
        }
        else
        {
            if (expr->index != 0)
            {
                i = expr->index;
                _commands->push_back("LOAD " + to_string(i));
            }
            i = get_var_index(variable);
        }
        _commands->push_back("STORE " + to_string(i));
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
        _commands->insert(_commands->end(), commands.begin(), commands.end());
        commands.clear();
        if (expr->type == PTR)
        {
            i = set_temp_ptr(expr)->index;
            _commands->insert(_commands->end(), commands.begin(), commands.end());
            commands.clear();
            _commands->push_back("LOADI " + to_string(i));
        }
        else
        {
            i = expr->index;
            _commands->push_back("LOAD " + to_string(i));
        }
        i = temp->index;
        _commands->push_back("STOREI " + to_string(i));
    }
    break;
    default:
        error("nieprawidlowa zmienna", lineno);
        break;
    }
    return _commands;
}

vecS *cmd_if(cond *condition, vecS *_commands, int lineno)
{
    vecS *_ifCom = new vecS();
    _ifCom->push_back("ZACZYNAM IFA");
    _ifCom->insert(_ifCom->end(), condition->commands.begin(), condition->commands.end());
    _ifCom->push_back("IFFFFFFFFFFFF COND OVER");
    _ifCom->insert(_ifCom->end(), _commands->begin(), _commands->end());
    _commands->clear();
    _ifCom->push_back("KONIEC IFA");
    return _ifCom;
}

vecS *cmd_read(var *current, int lineno)
{
    vecS *_commands = new vecS();
    if (current->type == VAR)
    {
        long long int i = getsym(current->name)->storedAt + current->index - getsym(current->name)->startIndex;
        _commands->push_back("GET");
        _commands->push_back("STORE " + to_string(i));
    }
    else if (current->type == PTR)
    {
        long long i = set_temp_ptr(current)->index;
        _commands->insert(_commands->end(), commands.begin(), commands.end());
        commands.clear();
        _commands->push_back("GET");
        _commands->push_back("STOREI " + to_string(i));
    }
    else
    {
        error("bledna zmienna", lineno);
    }
    return _commands;
}

vecS *cmd_write(var *current, int lineno)
{
    vecS *_commands = new vecS();
    switch (current->type)
    {
    case VAL:
    {
        long long int i = current->index;
        _commands->push_back("LOAD " + to_string(i));
    }
    break;
    case VAR:
    {
        long long int i = get_var_index(current);
        _commands->push_back("LOAD " + to_string(i));
    }
    break;
    case PTR:
    {
        var *temp = set_temp_ptr(current);
        _commands->insert(_commands->end(), commands.begin(), commands.end());
        commands.clear();
        long long int i = temp->index;
        _commands->push_back("LOADI " + to_string(i));
    }
    break;
    default:
        error("nieprawidlowa zmienna", lineno);
        break;
    }
    _commands->push_back("PUT");
    return _commands;
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
        return nullptr;
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
    return plus_minus(a, b, lineno, "ADD ");
}

var *expr_minus(var *a, var *b, int lineno)
{
    return plus_minus(a, b, lineno, "SUB ");
}

var *plus_minus(var *a, var *b, int lineno, string command)
{
    if (a->type == VAL)
    {
        if (b->type == VAL)
        {
            assign_to_p0(b->index);
            var *temp;
            temp = set_temp_var(nullptr);
            assign_to_p0(a->index);
            commands.push_back(command + to_string(temp->index));
            temp->index = 0;
            temp->type = VAL;
            return temp;
        }
        else if (b->type == VAR)
        {
            assign_to_p0(a->index);
            commands.push_back(command + to_string(get_var_index(b)));
            a->index = 0;
            return a;
        }
        else if (b->type == PTR)
        {
            var *ptr;
            ptr = set_temp_ptr(b);
            commands.push_back("LOADI " + to_string(ptr->index));
            var *temp;
            temp = set_temp_var(nullptr);
            assign_to_p0(a->index);
            commands.push_back(command + to_string(temp->index));
            temp->index = 0;
            temp->type = VAL;
            return temp;
        }
    }
    else if (a->type == VAR)
    {
        if (b->type == VAL)
        {
            assign_to_p0(b->index);
            var *temp;
            temp = set_temp_var(nullptr);
            commands.push_back("LOAD " + to_string(get_var_index(a)));
            commands.push_back(command + to_string(temp->index));
            b->index = 0;
            return b;
        }
        else if (b->type == VAR)
        {
            commands.push_back("LOAD " + to_string(get_var_index(a)));
            commands.push_back(command + to_string(get_var_index(b)));
            a->type = VAL;
            a->index = 0;
            return a;
        }
        else if (b->type == PTR)
        {
            var *ptr;
            ptr = set_temp_ptr(b);
            commands.push_back("LOADI " + to_string(ptr->index));
            var *temp;
            temp = set_temp_var(nullptr);
            commands.push_back("LOAD " + to_string(get_var_index(a)));
            commands.push_back(command + to_string(temp->index));
            a->type = VAL;
            a->index = 0;
            return a;
        }
    }
    else if (a->type == PTR)
    {
        if (b->type == VAL)
        {
            assign_to_p0(b->index);
            var *temp;
            temp = set_temp_var(nullptr);
            var *ptr;
            ptr = set_temp_ptr(a);
            commands.push_back("LOADI " + to_string(ptr->index));
            commands.push_back(command + to_string(temp->index));
            temp->index = 0;
            temp->type = VAL;
            return temp;
        }
        else if (b->type == VAR)
        {
            var *ptr;
            ptr = set_temp_ptr(a);
            commands.push_back("LOADI " + to_string(ptr->index));
            commands.push_back(command + to_string(get_var_index(b)));
            b->type = VAL;
            b->index = 0;
            return b;
        }
        else if (b->type == PTR)
        {
            var *ptrb;
            ptrb = set_temp_ptr(b);
            commands.push_back("LOADI " + to_string(ptrb->index));
            var *temp;
            temp = set_temp_var(nullptr);
            var *ptra;
            ptra = set_temp_ptr(a);
            commands.push_back("LOADI " + to_string(ptra->index));
            commands.push_back(command + to_string(temp->index));
            temp->index = 0;
            temp->type = VAL;
            return temp;
        }
    }
    error("nieprawidlowa zmienna", lineno);
    return nullptr;
}

cond *cond_eq(var *a, var *b, int lineno)
{
    return set_condtion(a, b, lineno, EQ);
}

cond *cond_neq(var *a, var *b, int lineno)
{
    return set_condtion(a, b, lineno, NEQ);
}
cond *cond_ge(var *a, var *b, int lineno)
{
    return set_condtion(a, b, lineno, GE);
}
cond *cond_geq(var *a, var *b, int lineno)
{
    return set_condtion(a, b, lineno, GEQ);
}

cond *set_condtion(var *a, var *b, int lineno, cond_type type)
{
    var *temp;
    temp = plus_minus(a, b, lineno, "SUB ");
    temp = set_temp_var(nullptr);
    cond *condition;
    condition = (cond *)malloc(sizeof(cond));
    condition->index = temp->index;
    condition->type = type;
    condition->commands.insert(condition->commands.end(), commands.begin(), commands.end());
    commands.clear();
    return condition;
}

void assign_to_p0(long long int value)
{
    commands.push_back("SUB 0");
    if (value != 0)
    {
        if (const_one == nullptr)
        {
            commands.push_back("INC");
            const_one = set_temp_var(const_one);
            commands.push_back("DEC");
        }

        string x = dec_to_bin(value > 0 ? value * (-1) : value);
        for (unsigned long long i = 0; i < x.length() - 1; i++)
        {
            if (x.at(i) == '1')
            {
                commands.push_back("INC");
            }
            commands.push_back("SHIFT " + to_string(const_one->index));
        }
        if (x.at(x.length() - 1) == '1')
        {
            commands.push_back("INC");
        }
        if (value < 0)
        {
            var *temp;
            temp = set_temp_var(nullptr);
            commands.push_back("SUB " + to_string(temp->index));
            commands.push_back("SUB " + to_string(temp->index));
        }
    }
}

string dec_to_bin(long long int num)
{
    string bin;
    while (num != 0)
    {
        bin = (num % 2 == 0 ? "0" : "1") + bin;
        num /= 2;
    }
    return bin;
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

void set_output_filename(char *filename)
{
    output_filename = filename;
}

void open_file()
{
    _file.open(output_filename);
}

void flush_to_file(vecS _commands)
{
    for (unsigned int cmd = 0; cmd < _commands.size(); cmd++)
    {
        _file << _commands.at(cmd) << endl;
    }
    _commands.clear();
}

void close_file()
{
    flush_to_file(commands);
    _file.close();
}
