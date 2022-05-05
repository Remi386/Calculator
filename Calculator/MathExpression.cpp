#include "MathExpression.h"
#include <regex>
#include <vector>

std::regex operators("[-*/+)(^]");

int HandleOperator(std::string& str, size_t offset)
{
    int spacesCount = 0;
    //After
    if (str.at(offset + 1) != ' ') {
        str.insert(offset + 1, 1, ' ');
        ++spacesCount;
    }
    //Before
    if (str.at(offset - 1) != ' ') {
        str.insert(offset, 1, ' ');
        ++spacesCount;
    }

    return spacesCount;
}

int HandleBracket(std::string& str, size_t offset)
{
    int spacesCount = 0;

    if (str[offset] == '(') {
        //After
        if (str.at(offset + 1) != ' ') {
            str.insert(offset + 1, 1, ' ');
            ++spacesCount;
        }
        //Before
        if (offset != 0) {
            str.insert(offset, 1, ' ');
            ++spacesCount;
        }
    }
    else {
        //After
        if (offset != str.size() - 1) {
            str.insert(offset + 1, 1, ' ');
            ++spacesCount;
        }
        //Before
        if (str.at(offset - 1) != ' ') {
            str.insert(offset, 1, ' ');
            ++spacesCount;
        }
    }
    return spacesCount;
}

//Adding spaces before and after operators/brackets
void AddSpaces(std::string& str)
{
    //we cant insert spaces and use regex iterators at the same string
    std::string copy(str);

    std::sregex_iterator oper_begin(str.begin(), str.end(), operators);
    std::sregex_iterator oper_end = std::sregex_iterator();

    //counting how many spaces we inserted to calculate offset
    int spacesCount = 0;

    while (oper_begin != oper_end)
    {
        std::smatch match = *oper_begin;
        size_t offset = match.position();

        try {
            if (str[offset] == '(' || str[offset] == ')')
                spacesCount += HandleBracket(copy, offset + spacesCount);
            else
                spacesCount += HandleOperator(copy, offset + spacesCount);
        }
        catch (...) {
            throw std::logic_error("Wrong position of operator");
        }
        ++oper_begin;
    }
    str = copy;
}

void AppropriateView(std::string& str)
{
    //preventing allocations
    str.reserve(str.size() * 2);

    //This space at the end quite useful in finding algorithms
    if (str[str.size() - 1] != ' ')
        str += ' ';

    AddSpaces(str);

    ////// Deleting double spaces
    size_t index = str.find("  ");

    while (index <= str.size() - 1)
    {
        str.replace(index, 2, " ");
        index = str.find("  ");
    }

    //deleting spaces in the beginning
    while (str[0] == ' ')
        str.erase(0, 1);

    str.shrink_to_fit();
}

bool IsOperator(std::string& str)
{
    if (str.size() > 1 || str.empty())
        return false;

    return std::regex_search(str, operators);
}

std::list<std::unique_ptr<Token>> Parse(std::string& expression)
{
    AppropriateView(expression);

    std::list<std::unique_ptr<Token>> output;
    std::list<Operator> operators;

    auto iterPrev = expression.begin();
    auto iterNext = std::find(expression.begin(), expression.end(), ' ');

    while (iterNext != expression.end())
    {
        std::string token(iterPrev, iterNext);
        if (IsOperator(token))
            AddOperator(output, operators, Operator(token));
        else
            output.push_back(std::make_unique<Value>(std::stod(token)));

        iterPrev = iterNext + 1;
        iterNext = std::find(iterPrev, expression.end(), ' ');
    }

    //Adding remaining operators
    while (!operators.empty())
    {
        if (operators.back().getOperator()[0] != '(') {
            output.push_back(std::make_unique<Operator>(operators.back()));
            operators.pop_back();
        }
        else {
            throw std::logic_error("Bracket mismatch: missing close bracket");
        }
    }

    return output;
}

float100 Calculate(std::list<std::unique_ptr<Token>> output)
{
    //There will be one item at the end
    while (output.size() != 1)
    {
        auto oper = std::find_if(output.begin(), output.end(),
            [](std::unique_ptr<Token>& token)
            {
                return token->isOperator();
            }
        );

        if (oper == output.end())
            throw std::logic_error("Not enough operators for values");

        //Not using std::prev because of bad error handling
        auto Decrement = [&output](std::list< std::unique_ptr<Token>>::iterator& iter) 
        {
            if (iter == output.begin())
                throw std::logic_error("Not enough values for operation");
            return --iter;
        };
        std::list< std::unique_ptr<Token>>::iterator tmp = oper;

        //Supports only binary operations
        std::list< std::unique_ptr<Token>>::iterator secondValue = Decrement(tmp);
        std::list< std::unique_ptr<Token>>::iterator firstValue = Decrement(tmp);

        if (!(*firstValue)->isOperator()
            && !(*secondValue)->isOperator())
        {
            std::unique_ptr<Token> result = std::make_unique<Value>(
                    DoOperation(*firstValue, *secondValue, *oper)
                );

            firstValue->swap(result);

            output.erase(oper);
            output.erase(secondValue);
        }
        else {
            throw std::logic_error("Wrong sequence!");
        }
    }

    return ((Value*)output.front().get())->getValue();
}

Value DoOperation(std::unique_ptr<Token>& firstValue,
                  std::unique_ptr<Token>& secondValue,
                  std::unique_ptr<Token>& _oper)
{
    auto& fValue = static_cast<Value*>(firstValue.get())->getValue();
    auto& sValue = static_cast<Value*>(secondValue.get())->getValue();
    auto& oper = static_cast<Operator*>(_oper.get())->getOperator();

    if (oper[0] == '+')
        return Value(fValue + sValue);

    if (oper[0] == '-')
        return Value(fValue - sValue);

    if (oper[0] == '*')
        return Value(fValue * sValue);

    if (oper[0] == '/')
        return Value(fValue / sValue);

    if (oper[0] == '^')
        return Value(boost::multiprecision::pow(fValue, sValue));
}

void AddOperator(std::list<std::unique_ptr<Token>>& output,
    std::list<Operator>& operators, Operator oper)
{
    if (oper.getOperator()[0] == ')') {
        while (true) {
            if (!operators.empty()) {
                if (operators.back().getOperator()[0] != '(')
                {
                    output.push_back(std::make_unique<Operator>(operators.back()));
                    operators.pop_back();
                }
                else {
                    operators.pop_back();
                    break;
                }
            }
            else {
                throw std::logic_error("Bracket mismatch: missing open bracket");
            }
        }
    }
    else {
        while (!operators.empty()
            && operators.back().getPriority() >= oper.getPriority() 
            && oper.getOperator()[0] != '(')
        {
            output.push_back(std::make_unique<Operator>(operators.back()));
            operators.pop_back();
        }
        operators.push_back(oper);
    }
}