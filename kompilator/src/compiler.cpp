#include "compiler.hpp"
#include "symbol-table.hpp"

vecS commands;
vecS front_commands;
vector<var *> temp;
vector<lVar *> locals;

struct var_init
{
    string name;
    long long int lineno;
};
vector<var_init> not_init;

var *const_one = nullptr;
var *const_minus_one = nullptr;

char *output_filename;
ofstream _file;

void error(string msg, int lineno)
{
    cerr << "Blad (linia " << lineno << "): " << msg << endl;
    cerr << "Kompilacja nieudana" << endl;
    exit(1);
}

void cmd_end(vecS *_commands)
{
    _commands->insert(_commands->begin(), front_commands.begin(), front_commands.end());
    front_commands.clear();
    _commands->push_back("HALT");
    flush_to_file(_commands);
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
    init_consts();
    if (variable->type == VAR)
    {

        if (local_exists(variable->name))
        {
            error("zmienna " + variable->name + " jest zmienna lokalna, a jej modyfikacja jest zabroniona", lineno);
        }
        else
        {
            set_init(variable->name);
            erase_init_variable(variable->name);
        }
    }
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
        }
        i = get_var_index(variable);
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
    init_consts();
    vecS *_ifCom = new vecS();
    vector<int> to_change;
    _ifCom->insert(_ifCom->end(), condition->commands.begin(), condition->commands.end());
    // _ifCom->push_back("LOAD " + to_string(condition->index));
    long long int cond_size = _ifCom->size();
    switch (condition->type)
    {
    case JEQ:
    {
        to_change.push_back(_ifCom->size());
        _ifCom->push_back("JPOS ");
        to_change.push_back(_ifCom->size());
        _ifCom->push_back("JNEG ");
    }
    break;
    case JNEQ:
    {
        to_change.push_back(_ifCom->size());
        _ifCom->push_back("JZERO ");
    }
    break;
    case JGE:
    {
        to_change.push_back(_ifCom->size());
        _ifCom->push_back("JNEG ");
        to_change.push_back(_ifCom->size());
        _ifCom->push_back("JZERO ");
    }
    break;
    case JGEQ:
    {
        to_change.push_back(_ifCom->size());
        _ifCom->push_back("JNEG ");
    }
    break;
    default:
        error("nieprawidlowa konstrukcja warunku", lineno);
        break;
    }
    _ifCom->insert(_ifCom->end(), _commands->begin(), _commands->end());
    _commands->clear();
    long long int size = _ifCom->size() - cond_size;
    for (unsigned int i = 0; i < to_change.size(); i++)
    {
        _ifCom->at(to_change.at(i)) += to_string(size);
        size--;
    }

    return _ifCom;
}

vecS *cmd_if_else(cond *condition, vecS *if_commands, vecS *else_commands, int lineno)
{
    init_consts();
    vecS *_ifCom = new vecS();
    vector<int> to_change;
    _ifCom->insert(_ifCom->end(), condition->commands.begin(), condition->commands.end());
    // _ifCom->push_back("LOAD " + to_string(condition->index));
    long long int cond_size = _ifCom->size();
    switch (condition->type)
    {
    case JEQ:
    {
        to_change.push_back(_ifCom->size());
        _ifCom->push_back("JPOS ");
        to_change.push_back(_ifCom->size());
        _ifCom->push_back("JNEG ");
    }
    break;
    case JNEQ:
    {
        to_change.push_back(_ifCom->size());
        _ifCom->push_back("JZERO ");
    }
    break;
    case JGE:
    {
        to_change.push_back(_ifCom->size());
        _ifCom->push_back("JNEG ");
        to_change.push_back(_ifCom->size());
        _ifCom->push_back("JZERO ");
    }
    break;
    case JGEQ:
    {
        to_change.push_back(_ifCom->size());
        _ifCom->push_back("JNEG ");
    }
    break;
    default:
        error("nieprawidlowa konstrukcja warunku", lineno);
        break;
    }
    _ifCom->insert(_ifCom->end(), if_commands->begin(), if_commands->end());
    if_commands->clear();
    long long int size = _ifCom->size() - cond_size + 1;
    for (unsigned int i = 0; i < to_change.size(); i++)
    {
        _ifCom->at(to_change.at(i)) += to_string(size);
        size--;
    }
    size = else_commands->size() + 1;
    _ifCom->push_back("JUMP " + to_string(size));
    _ifCom->insert(_ifCom->end(), else_commands->begin(), else_commands->end());
    return _ifCom;
}

vecS *cmd_while(cond *condition, vecS *_commands, int lineno)
{
    init_consts();
    vecS *_ifCom = new vecS();
    vector<int> to_change;
    _ifCom->insert(_ifCom->end(), condition->commands.begin(), condition->commands.end());
    // _ifCom->push_back("LOAD " + to_string(condition->index));
    long long int cond_size = _ifCom->size();
    switch (condition->type)
    {
    case JEQ:
    {
        to_change.push_back(_ifCom->size());
        _ifCom->push_back("JPOS ");
        to_change.push_back(_ifCom->size());
        _ifCom->push_back("JNEG ");
    }
    break;
    case JNEQ:
    {
        to_change.push_back(_ifCom->size());
        _ifCom->push_back("JZERO ");
    }
    break;
    case JGE:
    {
        to_change.push_back(_ifCom->size());
        _ifCom->push_back("JNEG ");
        to_change.push_back(_ifCom->size());
        _ifCom->push_back("JZERO ");
    }
    break;
    case JGEQ:
    {
        to_change.push_back(_ifCom->size());
        _ifCom->push_back("JNEG ");
    }
    break;
    default:
        error("nieprawidlowa konstrukcja warunku", lineno);
        break;
    }
    _ifCom->insert(_ifCom->end(), _commands->begin(), _commands->end());
    _commands->clear();

    condition = change_condition(condition, lineno);
    _ifCom->insert(_ifCom->end(), condition->commands.begin(), condition->commands.end());
    condition->commands.clear();

    long long int size = _ifCom->size() - cond_size + 1;
    for (unsigned int i = 0; i < to_change.size(); i++)
    {
        _ifCom->at(to_change.at(i)) += to_string(size);
        size--;
    }
    size = _ifCom->size() - cond_size;
    size *= -1;
    _ifCom->push_back("JUMP " + to_string(size));
    return _ifCom;
}

vecS *cmd_do_while(cond *condition, vecS *_commands, int lineno)
{
    init_consts();
    vecS *_ifCom = new vecS();
    _ifCom->insert(_ifCom->end(), _commands->begin(), _commands->end());
    _commands->clear();

    condition = change_condition(condition, lineno);
    _ifCom->insert(_ifCom->end(), condition->commands.begin(), condition->commands.end());
    condition->commands.clear();
    long long int size = _ifCom->size() * (-1);
    string idx = to_string(size);
    switch (condition->type)
    {
    case JEQ:
    {
        _ifCom->push_back("JZERO " + idx);
    }
    break;
    case JNEQ:
    {
        _ifCom->push_back("JPOS " + idx);
        _ifCom->push_back("JNEG " + idx);
    }
    break;
    case JGE:
    {
        _ifCom->push_back("JPOS " + idx);
    }
    break;
    case JGEQ:
    {
        _ifCom->push_back("JPOS " + idx);
        _ifCom->push_back("JZERO " + idx);
    }
    break;
    default:
        error("nieprawidlowa konstrukcja warunku", lineno);
        break;
    }
    return _ifCom;
}

void cmd_for_init(string iterator, int lineno)
{
    init_consts();
    if (symbol_exists(iterator))
    {
        error("zmienna " + iterator + " zostala juz zadeklarowana i nie moze byc uzyta w petli for", lineno);
    }
    else
    {
        if (local_exists(iterator))
        {
            error("zmienna " + iterator + " jest juz uzywana", lineno);
        }
        else
        {
            set_local_variable(iterator);
        }
    }
}

vecS *cmd_for(string iterator, var *_from, var *_to, vecS *_commands, int lineno)
{
    init_consts();
    vecS *_forCom = new vecS();
    lVar *_i;
    _i = get_local_variable(iterator);
    if (_i == nullptr)
    {
        error("zmienna " + iterator + " nie jest zmienna lokalna", lineno);
    }
    var *b;
    switch (_to->type)
    {
    case VAL:
        assign_to_p0(_to->index);
        break;
    case VAR:
        commands.push_back("LOAD " + to_string(get_var_index(_to)));
        break;
    case PTR:
    {
        var *ptr;
        ptr = set_temp_ptr(_to);
        commands.push_back("LOADI " + to_string(ptr->index));
    }
    break;
    default:
        error("nieprawidlowa zmienna konczaca petle", lineno);
        break;
    }
    b = set_temp_var(nullptr);
    plus_minus(_to, _from, lineno, "SUB ");
    _forCom->insert(_forCom->end(), commands.begin(), commands.end());
    commands.clear();

    _forCom->push_back("STORE " + to_string(_i->index));
    int jneg_index = _forCom->size();
    _forCom->push_back("JNEG ");
    _forCom->push_back("LOAD " + to_string(b->index));
    _forCom->push_back("SUB " + to_string(_i->index));
    _forCom->push_back("STORE " + to_string(_i->index));

    _forCom->insert(_forCom->end(), _commands->begin(), _commands->end());
    _commands->clear();

    _forCom->push_back("LOAD " + to_string(b->index));
    _forCom->push_back("SUB " + to_string(_i->index));
    if (const_one == nullptr)
    {
        commands.push_back("INC");
        const_one = set_temp_var(const_one);
        commands.push_back("DEC");
    }
    _forCom->insert(_forCom->end(), commands.begin(), commands.end());
    commands.clear();
    _forCom->push_back("SUB " + to_string(const_one->index));
    _forCom->push_back("STORE " + to_string(_i->index));
    long int size = _forCom->size() - jneg_index;
    _forCom->push_back("JUMP " + to_string(size * (-1)));
    size++;
    _forCom->at(jneg_index) += to_string(size);
    erase_local_variable(_i->name);
    return _forCom;
}

vecS *cmd_for_downto(string iterator, var *_from, var *_downto, vecS *_commands, int lineno)
{
    init_consts();
    vecS *_forCom = new vecS();
    lVar *_i;
    _i = get_local_variable(iterator);
    if (_i == nullptr)
    {
        error("zmienna " + iterator + " nie jest zmienna lokalna", lineno);
    }
    var *b;
    switch (_downto->type)
    {
    case VAL:
        assign_to_p0(_downto->index);
        break;
    case VAR:
        commands.push_back("LOAD " + to_string(get_var_index(_downto)));
        break;
    case PTR:
    {
        var *ptr;
        ptr = set_temp_ptr(_downto);
        commands.push_back("LOADI " + to_string(ptr->index));
    }
    break;
    default:
        error("nieprawidlowa zmienna konczaca petle", lineno);
        break;
    }
    b = set_temp_var(nullptr);
    plus_minus(_from, _downto, lineno, "SUB ");
    _forCom->insert(_forCom->end(), commands.begin(), commands.end());
    commands.clear();

    _forCom->push_back("STORE " + to_string(_i->index));
    int jneg_index = _forCom->size();
    _forCom->push_back("JNEG ");
    _forCom->push_back("LOAD " + to_string(b->index));
    _forCom->push_back("ADD " + to_string(_i->index));
    _forCom->push_back("STORE " + to_string(_i->index));

    _forCom->insert(_forCom->end(), _commands->begin(), _commands->end());
    _commands->clear();

    _forCom->push_back("LOAD " + to_string(_i->index));
    _forCom->push_back("SUB " + to_string(b->index));
    if (const_one == nullptr)
    {
        commands.push_back("INC");
        const_one = set_temp_var(const_one);
        commands.push_back("DEC");
    }
    _forCom->insert(_forCom->end(), commands.begin(), commands.end());
    commands.clear();
    _forCom->push_back("SUB " + to_string(const_one->index));
    _forCom->push_back("STORE " + to_string(_i->index));
    long int size = _forCom->size() - jneg_index;
    _forCom->push_back("JUMP " + to_string(size * (-1)));
    size++;
    _forCom->at(jneg_index) += to_string(size);
    erase_local_variable(_i->name);
    return _forCom;
}

vecS *cmd_read(var *current, int lineno)
{
    init_consts();
    if (local_exists(current->name))
    {
        error("zmienna " + current->name + " jest zmienna lokalna, a jej modyfikacja jest zabroniona", lineno);
    }
    vecS *_commands = new vecS();
    if (current->type == VAR)
    {
        set_init(current->name);
        erase_local_variable(current->name);
        long long int i = get_var_index(current);
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
    init_consts();
    // check_init(current->name, lineno);
    vecS *_commands = new vecS();
    switch (current->type)
    {
    case VAL:
    {
        assign_to_p0(current->index);
        _commands->insert(_commands->end(), commands.begin(), commands.end());
    }
    break;
    case VAR:
    {
        _commands->push_back("LOAD " + to_string(get_var_index(current)));
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

var *expr_val(var *value, int lineno)
{
    init_consts();
    // check_init(value->name, lineno);
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
        value->index = get_var_index(value);
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
    init_consts();
    // check_init(a->name, lineno);
    // check_init(b->name, lineno);
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

var *expr_times(var *a, var *b, int lineno)
{
    init_consts();
    // check_init(a->name, lineno);
    // check_init(b->name, lineno);
    var *tempA;
    var *tempB;
    var *res;
    var *sign;
    assign_to_p0(0);
    res = set_temp_var(nullptr);
    sign = set_temp_var(nullptr);
    switch (a->type)
    {
    case VAL:
        assign_to_p0(a->index);
        break;
    case VAR:
        commands.push_back("LOAD " + to_string(get_var_index(a)));
        break;
    case PTR:
    {
        var *ptr;
        ptr = set_temp_ptr(a);
        commands.push_back("LOADI " + to_string(ptr->index));
    }
    break;
    default:
        error("nieprawidlowa zmienna", lineno);
        break;
    }
    tempA = set_temp_var(nullptr);
    switch (b->type)
    {
    case VAL:
        assign_to_p0(b->index);
        break;
    case VAR:
        commands.push_back("LOAD " + to_string(get_var_index(b)));
        break;
    case PTR:
    {
        var *ptr;
        ptr = set_temp_ptr(b);
        commands.push_back("LOADI " + to_string(ptr->index));
    }
    break;
    default:
        error("nieprawidlowa zmienna", lineno);
        break;
    }
    tempB = set_temp_var(nullptr);
    string _a = to_string(tempA->index);
    string _b = to_string(tempB->index);
    string _res = to_string(res->index);
    string _sign = to_string(sign->index);

    // sign saving
    commands.push_back("LOAD " + _b);
    commands.push_back("JZERO 31");
    commands.push_back("LOAD " + _a);
    commands.push_back("JZERO 29");
    commands.push_back("JPOS 7");
    commands.push_back("LOAD " + to_string(const_one->index));
    commands.push_back("STORE " + _sign);
    commands.push_back("LOAD " + _a);
    commands.push_back("SUB " + _a);
    commands.push_back("SUB " + _a);
    commands.push_back("STORE " + _a);

    // multiplication
    commands.push_back("JZERO 15");
    commands.push_back("SHIFT " + to_string(const_minus_one->index));
    commands.push_back("SHIFT " + to_string(const_one->index));
    commands.push_back("SUB " + _a);
    commands.push_back("JZERO 4");
    commands.push_back("LOAD " + _res);
    commands.push_back("ADD " + _b);
    commands.push_back("STORE " + _res);
    commands.push_back("LOAD " + _b);
    commands.push_back("ADD " + _b);
    commands.push_back("STORE " + _b);
    commands.push_back("LOAD " + _a);
    commands.push_back("SHIFT " + to_string(const_minus_one->index));
    commands.push_back("STORE " + _a);
    commands.push_back("JUMP -14");

    // sign check
    commands.push_back("LOAD " + _sign);
    commands.push_back("JZERO 5");
    commands.push_back("LOAD " + _res);
    commands.push_back("SUB " + _res);
    commands.push_back("SUB " + _res);
    commands.push_back("STORE " + _res);

    return res;
}

var *expr_div(var *a, var *b, int lineno)
{
    return div_mod(a, b, lineno, true);
}

var *expr_mod(var *a, var *b, int lineno)
{
    return div_mod(a, b, lineno, false);
}

var *div_mod(var *a, var *b, int lineno, bool do_div)
{
    init_consts();
    // check_init(a->name, lineno);
    // check_init(b->name, lineno);
    var *res;
    assign_to_p0(0);
    res = set_temp_var(nullptr);
    var *mul;
    assign_to_p0(1);
    mul = set_temp_var(nullptr);
    var *_a;
    var *_b;
    var *tempA;
    switch (a->type)
    {
    case VAL:
        assign_to_p0(a->index);
        break;
    case VAR:
        commands.push_back("LOAD " + to_string(get_var_index(a)));
        break;
    case PTR:
    {
        var *ptr;
        ptr = set_temp_ptr(a);
        commands.push_back("LOADI " + to_string(ptr->index));
    }
    break;
    default:
        error("nieprawidlowa zmienna", lineno);
        break;
    }
    _a = set_temp_var(nullptr);
    tempA = set_temp_var(nullptr);
    a = set_temp_var(a);
    switch (b->type)
    {
    case VAL:
        assign_to_p0(b->index);
        break;
    case VAR:
        commands.push_back("LOAD " + to_string(get_var_index(b)));
        break;
    case PTR:
    {
        var *ptr;
        ptr = set_temp_ptr(b);
        commands.push_back("LOADI " + to_string(ptr->index));
    }
    break;
    default:
        error("nieprawidlowa zmienna", lineno);
        break;
    }
    _b = set_temp_var(nullptr);
    b = set_temp_var(b);
    string s_a = to_string(_a->index);
    string s_b = to_string(_b->index);
    string s_res = to_string(res->index);
    string s_mul = to_string(mul->index);
    string s_a_ = to_string(tempA->index);

    if (do_div)
    {
        // changing sign to +
        commands.push_back("LOAD " + s_a);
        commands.push_back("JZERO 57"); //out
        commands.push_back("JPOS 5");
        commands.push_back("SUB " + s_a);
        commands.push_back("SUB " + s_a);
        commands.push_back("STORE " + s_a);
        commands.push_back("STORE " + s_a_);
        commands.push_back("LOAD " + s_b);
        commands.push_back("JZERO 50"); //out
        commands.push_back("JPOS 4");
        commands.push_back("SUB " + s_b);
        commands.push_back("SUB " + s_b);
        commands.push_back("STORE " + s_b);
    }
    else
    {
        commands.push_back("LOAD " + s_b);
        commands.push_back("JPOS 9");
        commands.push_back("JNEG 5");
        commands.push_back("LOAD " + s_a);
        commands.push_back("SUB " + s_a);
        commands.push_back("STORE " + s_a);
        commands.push_back("JUMP 55"); //out
        commands.push_back("SUB " + s_b);
        commands.push_back("SUB " + s_b);
        commands.push_back("STORE " + s_b);
        commands.push_back("LOAD " + s_a);
        commands.push_back("JZERO 50"); //out
        commands.push_back("JPOS 5");
        commands.push_back("SUB " + s_a);
        commands.push_back("SUB " + s_a);
        commands.push_back("STORE " + s_a);
        commands.push_back("STORE " + s_a_);
    }

    // division
    commands.push_back("LOAD " + s_b);
    commands.push_back("SUB " + s_a_);
    commands.push_back("JPOS 10");
    commands.push_back("JZERO 9");
    commands.push_back("LOAD " + s_mul);
    commands.push_back("SHIFT " + to_string(const_one->index));
    commands.push_back("STORE " + s_mul);
    commands.push_back("LOAD " + s_b);
    commands.push_back("SHIFT " + to_string(const_one->index));
    commands.push_back("STORE " + s_b);
    commands.push_back("SUB " + s_a_);
    commands.push_back("JUMP -9");

    commands.push_back("LOAD " + s_a);
    commands.push_back("SUB " + s_b);
    commands.push_back("JNEG 7");
    commands.push_back("LOAD " + s_a);
    commands.push_back("SUB " + s_b);
    commands.push_back("STORE " + s_a);
    commands.push_back("LOAD " + s_res);
    commands.push_back("ADD " + s_mul);
    commands.push_back("STORE " + s_res);

    commands.push_back("LOAD " + s_b);
    commands.push_back("SHIFT " + to_string(const_minus_one->index));
    commands.push_back("STORE " + s_b);
    commands.push_back("LOAD " + s_mul);
    commands.push_back("SHIFT " + to_string(const_minus_one->index));
    commands.push_back("STORE " + s_mul);
    commands.push_back("JPOS -15");
    commands.push_back("JNEG -16");

    s_a = to_string(a->index);
    s_b = to_string(b->index);
    s_a_ = to_string(_a->index);
    if (do_div)
    {
        // result sign change
        commands.push_back("LOAD " + s_b);
        commands.push_back("JNEG 4");
        commands.push_back("LOAD " + s_a);
        commands.push_back("JPOS 13"); //out
        commands.push_back("JNEG 3");
        commands.push_back("LOAD " + s_a);
        commands.push_back("JNEG 10"); //out

        commands.push_back("LOAD " + s_a_);
        commands.push_back("JZERO 4");
        commands.push_back("LOAD " + s_res);
        commands.push_back("ADD " + to_string(const_one->index));
        commands.push_back("STORE " + s_res);

        commands.push_back("LOAD " + s_res);
        commands.push_back("SUB " + s_res);
        commands.push_back("SUB " + s_res);
        commands.push_back("STORE " + s_res);

        return res;
    }
    else
    {
        commands.push_back("LOAD " + s_a_);
        commands.push_back("JZERO 14");
        commands.push_back("LOAD " + s_a);
        commands.push_back("JNEG 4");
        commands.push_back("LOAD " + s_b);
        commands.push_back("JPOS 10"); //out
        commands.push_back("JNEG 7");
        commands.push_back("LOAD " + s_a_);
        commands.push_back("SUB " + s_a_);
        commands.push_back("SUB " + s_a_);
        commands.push_back("STORE " + s_a_);
        commands.push_back("LOAD " + s_b);
        commands.push_back("JNEG 3"); //out
        commands.push_back("ADD " + s_a_);
        commands.push_back("STORE " + s_a_);
        return _a;
    }
}

cond *cond_eq(var *a, var *b, int lineno)
{
    return set_condition(a, b, lineno, JEQ);
}

cond *cond_neq(var *a, var *b, int lineno)
{
    return set_condition(a, b, lineno, JNEQ);
}
cond *cond_ge(var *a, var *b, int lineno)
{
    return set_condition(a, b, lineno, JGE);
}
cond *cond_geq(var *a, var *b, int lineno)
{
    return set_condition(a, b, lineno, JGEQ);
}

cond *set_condition(var *a, var *b, int lineno, cond_type type)
{
    init_consts();
    // check_init(a->name, lineno);
    // check_init(b->name, lineno);
    cond *condition;
    condition = new cond;
    condition->sourceA = a;
    condition->sourceB = b;
    var *temp;
    var tempA = *a;
    var tempB = *b;
    temp = plus_minus(&tempA, &tempB, lineno, "SUB ");
    temp = set_temp_var(nullptr);
    condition->index = temp->index;
    condition->type = type;
    condition->commands.insert(condition->commands.end(), commands.begin(), commands.end());
    commands.clear();
    condition->commands.pop_back();
    return condition;
}

cond *change_condition(cond *condition, int lineno)
{
    init_consts();
    var tempA = *condition->sourceA;
    var tempB = *condition->sourceB;
    plus_minus(&tempA, &tempB, lineno, "SUB ");
    condition->commands.clear();
    condition->commands.insert(condition->commands.end(), commands.begin(), commands.end());
    commands.clear();
    return condition;
}

var *cmd_num(long long int value, int lineno)
{
    var *current_var;
    current_var = new var;
    current_var->index = value;
    current_var->type = VAL;
    return current_var;
}

var *cmd_id(var *variable, int lineno)
{
    if (variable->type == VAR && !local_exists(variable->name))
    {
        if (!is_init(variable->name))
        {
            bool found = false;
            for (unsigned long long int i = 0; i < not_init.size() && !found; i++)
            {
                if (not_init.at(i).name.compare(variable->name) == 0)
                {
                    found = true;
                }
            }
            if (!found)
            {
                var_init v;
                v.name = variable->name;
                v.lineno = lineno;
                not_init.push_back(v);
            }
            // error("zmienna " + variable->name + " nie zostala zainicjalizowana", lineno);
        }
    }
    return variable;
}

// variable
var *cmd_pid(string name, int lineno)
{
    symrec *s;
    s = getsym(name);
    if (s == 0)
    {
        if (!local_exists(name))
        {
            error("zmienna " + name + " nie zostala zadeklarowana", lineno);
            return nullptr;
        }
        else
        {
            return set_local_variable(name);
        }
    }
    else if (s->type == ARRAY)
    {
        error("zmienna " + name + " jest zmienna tablicowa", lineno);
        return nullptr;
    }
    else
    {
        var *current_var;
        current_var = new var;
        current_var->name = name;
        current_var->index = (long long int)1;
        current_var->type = VAR;
        return current_var;
    }
}

// array
var *cmd_pid(string name, long long int index, int lineno)
{
    symrec *s;
    s = getsym(name);
    if (s == 0)
    {
        error("zmienna " + name + " nie zostala zadeklarowana", lineno);
        return nullptr;
    }
    else if (s->type != ARRAY)
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
        current_var = new var;
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
        error("zmienna " + name + " nie zostala zadeklarowana", lineno);
        return nullptr;
    }
    else
    {
        if (!symbol_exists(indexName))
        {

            if (!local_exists(indexName))
            {
                error("zmienna " + indexName + " nie zostala zadeklarowana", lineno);
                return nullptr;
            }
            else
            {
                set_local_variable(indexName);
            }
        }
        var *current_var;
        current_var = new var;
        current_var->name = name;
        current_var->index = s->storedAt - s->startIndex;
        current_var->indexName = indexName;
        current_var->type = PTR;
        return current_var;
    }
}

void assign_to_p0(long long int value)
{
    commands.push_back("SUB 0");
    if (const_one == nullptr)
    {
        commands.push_back("INC");
        const_one = set_temp_var(const_one);
        commands.push_back("DEC");
    }
    if (const_minus_one == nullptr)
    {
        commands.push_back("DEC");
        const_minus_one = set_temp_var(const_minus_one);
        commands.push_back("INC");
    }
    if (value != 0)
    {

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
        variable = new var;
        variable->type = VAL;
    }

    long long int i = get_offset() + temp.size();
    variable->index = i;
    temp.push_back(variable);
    commands.push_back("STORE " + to_string(i));
    return variable;
}

void change_temp_var(long long int index, vecS *_commands)
{
    index -= get_offset();
    long long int i = temp.at(index)->index;
    _commands->push_back("STORE " + to_string(i));
}

var *set_temp_ptr(var *current)
{
    assign_to_p0(current->index);
    symrec *s;
    s = getsym(current->indexName);
    long long int index;
    if (s == 0)
    {
        lVar *l_var;
        l_var = get_local_variable(current->indexName);
        index = l_var->index;
    }
    else
    {
        index = s->storedAt;
    }
    commands.push_back("ADD " + to_string(index));
    return set_temp_var(nullptr);
}

long long int get_var_index(var *current)
{
    symrec *s;
    s = getsym(current->name);
    if (s != 0)
    {
        return s->storedAt + current->index - s->startIndex;
    }
    else
    {
        lVar *l_var;
        l_var = get_local_variable(current->name);
        return l_var->index;
    }
}

void check_jumps(vecS *_commands)
{
    for (unsigned long long int i = 0; i < _commands->size(); i++)
    {
        string label = _commands->at(i);
        bool jump_found = label.find("JPOS") != string::npos || label.find("JZERO") != string::npos || label.find("JNEG") != string::npos || label.find("JUMP") != string::npos;
        if (jump_found)
        {
            string cmd = label.substr(0, label.find(" "));
            long long int k = stoll(label.substr(label.rfind(" ")));
            k += i;
            _commands->at(i) = cmd + " " + to_string(k);
        }
    }
}

void check_inits()
{
    if (!not_init.empty())
    {
        var_init v = not_init.front();
        error("zmienna " + v.name + " nie zostala zainicjalizowana", v.lineno);
    }
}

var *set_local_variable(string name)
{
    var *loc;
    loc = new var;
    loc->type = VAR;
    lVar *ptr;
    ptr = get_local_variable(name);
    if (ptr == nullptr)
    {
        loc->name = name;
        long long int i = get_offset() + temp.size();
        loc->index = i;
        temp.push_back(loc);
        ptr = new lVar;
        ptr->name = loc->name;
        ptr->index = loc->index;
        locals.push_back(ptr);
    }
    else
    {
        loc->name = ptr->name;
        loc->index = ptr->index;
    }
    return loc;
}

lVar *get_local_variable(string name)
{
    for (unsigned long long int i = 0; i < locals.size(); i++)
    {
        if (locals.at(i)->name.compare(name) == 0)
        {
            return locals.at(i);
        }
    }
    return nullptr;
}

void erase_local_variable(string name)
{
    for (unsigned long long int i = 0; i < locals.size(); i++)
    {
        if (locals.at(i)->name.compare(name) == 0)
        {
            locals.erase(locals.begin() + i);
        }
    }
}

void erase_init_variable(string name)
{
    for (unsigned long long int i = 0; i < not_init.size(); i++)
    {
        if (not_init.at(i).name.compare(name) == 0)
        {
            not_init.erase(not_init.begin() + i);
        }
    }
}

bool local_exists(string name)
{
    for (unsigned long long int i = 0; i < locals.size(); i++)
    {
        if (locals.at(i)->name.compare(name) == 0)
        {
            return true;
        }
    }
    return false;
}

void init_consts()
{
    if (const_one == nullptr)
    {
        commands.push_back("SUB 0");
        commands.push_back("INC");
        const_one = set_temp_var(const_one);
        front_commands.insert(front_commands.end(), commands.begin(), commands.end());
        commands.clear();
    }
    if (const_minus_one == nullptr)
    {
        commands.push_back("SUB 0");
        commands.push_back("DEC");
        const_minus_one = set_temp_var(const_minus_one);
        front_commands.insert(front_commands.end(), commands.begin(), commands.end());
        commands.clear();
    }
}

void set_output_filename(char *filename)
{
    output_filename = filename;
}

void open_file()
{
    _file.open(output_filename);
}

void flush_to_file(vecS *_commands)
{
    check_inits();
    check_jumps(_commands);
    for (unsigned int cmd = 0; cmd < _commands->size(); cmd++)
    {
        _file << _commands->at(cmd) << endl;
    }
    _commands->clear();
}

void close_file()
{
    _file.close();
}
