#include "MathExpression.h"
#include <regex>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <boost/function.hpp>
#include <string>
#include <iostream>

using OperType = Operator::OperatorType;
using OperPos = Operator::Position;

struct BinaryOperatorInformation
{
    short operatorPriority = 0;
    OperType operatorType = OperType::unary;
    boost::function<float100(float100, float100)> functor;
};

struct UnaryOperatorInformation
{
    short operatorPriority;
    OperType operatorType;
    OperPos operatorPosition;
    boost::function<float100(float100)> functor;
};

const std::unordered_map<std::string_view, BinaryOperatorInformation> binaryOperators = 
{
    {"+", { 0, OperType::binary, [](const float100& lhs, const float100& rhs) { return lhs + rhs; } } },
    {"-", { 0, OperType::binary, [](const float100& lhs, const float100& rhs) { return lhs - rhs; } } },
    {"*", { 1, OperType::binary, [](const float100& lhs, const float100& rhs) { return lhs * rhs; } } },
    {"/", { 1, OperType::binary, [](const float100& lhs, const float100& rhs) { return lhs / rhs; } } },
    {"^", { 2, OperType::binary, [](const float100& lhs, const float100& rhs) { return boost::multiprecision::pow(lhs, rhs); } } }
};

const std::unordered_map<std::string_view, UnaryOperatorInformation> unaryOperators =
{
    {"log",  { 3, OperType::unary, OperPos::prefix,  [](const float100& value) { return boost::multiprecision::log10(value); } } },
    {"ln",   { 3, OperType::unary, OperPos::prefix,  [](const float100& value) { return boost::multiprecision::log(value);   } } },
    {"sqrt", { 3, OperType::unary, OperPos::prefix,  [](const float100& value) { return boost::multiprecision::sqrt(value);  } } },
    {"sin",  { 3, OperType::unary, OperPos::prefix,  [](const float100& value) { return boost::multiprecision::sin(value);   } } },
    {"cos",  { 3, OperType::unary, OperPos::prefix,  [](const float100& value) { return boost::multiprecision::cos(value);   } } },
    {"tan",  { 3, OperType::unary, OperPos::prefix,  [](const float100& value) { return boost::multiprecision::tan(value);   } } },
    {"!",    { 3, OperType::unary, OperPos::postfix, [](const float100& value) { return boost::multiprecision::tgamma(value + 1); } } },
    {"-",    { 4, OperType::unary, OperPos::prefix,  [](const float100& value) { return -value; } } }
};

std::regex InitOperators() 
{
    std::string operators;
    std::unordered_set<char> oneCharOperators;

    auto addOperatorNamesToRegex = [&](const auto& mapPair) 
    {
        if (mapPair.first.size() == 1)
            oneCharOperators.insert(mapPair.first[0]);
        else
        {
            operators.append(mapPair.first);
            operators.push_back('|');
        }
    };

    std::for_each(binaryOperators.begin(), binaryOperators.end(), addOperatorNamesToRegex);
    std::for_each(unaryOperators.begin(),  unaryOperators.end(),  addOperatorNamesToRegex);

    if (oneCharOperators.size() > 0) {
        operators.push_back('[');
        
        operators.append("\\)\\(");
        
        for (const char charOperator : oneCharOperators) {
            operators.push_back('\\'); //Escape them
            operators.push_back(charOperator);
        }
        
        operators.push_back(']');
    }


    return std::regex(operators);
}

const std::regex allOperatorsRegex = InitOperators();

Operator::Operator(std::string s, OperatorType _type)
    : oper(std::move(s)), operType(_type)
{
    if (operType == OperatorType::binary) {
        if (binaryOperators.contains(oper))
            priority = binaryOperators.at(oper).operatorPriority;
    }
    else if (operType == OperatorType::unary) {

        if (unaryOperators.contains(oper)) {
            priority = unaryOperators.at(oper).operatorPriority;
            position = unaryOperators.at(oper).operatorPosition;
        }
    }
    else { //brackets
        priority = -1;
    }

    if (priority == -10) //if priority has default value
        throw std::logic_error("Wrong operator: " + oper);
}

Operator MathExpression::MakeOperator(const std::string& str, bool isPrevOperator)
{
    //unary minus
    if (isPrevOperator && str == "-")
        return Operator(str, OperType::unary);

    if (str == "(")
        return Operator(str, OperType::openBracket);

    if (str == ")")
        return Operator(str, OperType::closeBracket);

    if (binaryOperators.contains(str))
        return Operator(str, OperType::binary);

    if (unaryOperators.contains(str))
        return Operator(str, OperType::unary);

    throw std::logic_error("Wrong operator: " + str);
}

int MathExpression::HandleToken(std::string& str, size_t offset, size_t length)
{
    int spacesCount = 0;

    if (offset + length + 1 < str.size() && str[offset + length + 1] != ' ') {
        str.insert(offset + length + 1, 1, ' ');
        ++spacesCount;
    }

    if (offset >= 1 && str[offset - 1] != ' ') {
        str.insert(offset, 1, ' ');
        ++spacesCount;
    }
    return spacesCount;
}

//Adding spaces before and after operators/brackets
void MathExpression::AddSpaces(std::string& str)
{
    //we cant insert spaces and use regex iterators in the same string
    std::string copy(str);

    std::sregex_iterator oper_begin(str.begin(), str.end(), allOperatorsRegex);
    std::sregex_iterator oper_end = std::sregex_iterator();

    //counting how many spaces we inserted to calculate offset
    int spacesCount = 0;

    while (oper_begin != oper_end)
    {
        std::smatch match = *oper_begin;

        size_t offset = match.position();

        size_t length = match.length() - 1;
        //length = getOperatorLength(str, offset);

        spacesCount += HandleToken(copy, offset + spacesCount, length);

        ++oper_begin;
    }
    str = copy;
}

void MathExpression::AppropriateView(std::string& str)
{
    if (str.empty())
        return;

    //preventing allocations
    str.reserve(str.size() * 2 + 2);

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

    //deleting spaces at the beginning
    while (str[0] == ' ')
        str.erase(0, 1);

    str.shrink_to_fit();
}

bool MathExpression::IsSupportedOperator(const std::string& str)
{
    return std::regex_search(str, allOperatorsRegex);
}

std::list<std::unique_ptr<Token>> 
    MathExpression::Parse(std::string& expression)
{
    AppropriateView(expression);

    std::list<std::unique_ptr<Token>> output;
    std::list<Operator> operators;

    if (expression.empty())
        output.push_back(std::make_unique<Value>(0.0));

    enum class tokens {
        Operator,
        Value,
        None
    } prev = tokens::None, needNext = tokens::None;

    bool isPrevOperator = true;

    auto iterPrev = expression.begin();
    auto iterNext = std::find(expression.begin(), expression.end(), ' ');

    while (iterNext != expression.end())
    {
        std::string token(iterPrev, iterNext);
        if (IsSupportedOperator(token)) {
            Operator oper = MakeOperator(token, isPrevOperator);

            //Checking an expression for correctness in infis notation
            if (oper.Type() == OperType::unary) {
                if (oper.Pos() == OperPos::postfix) {
                    if (prev != tokens::Value)
                        throw std::logic_error("Didn't found value for operator: " + token);
                }
                else { //is prefix operator
                    if (prev == tokens::Value)
                        throw std::logic_error("Expected binary operator, but found: " + token);

                    prev = tokens::Operator;
                    needNext = tokens::Value;
                }
            }
            else if (oper.Type() == OperType::binary) {

                if (prev == tokens::Operator)
                    throw std::logic_error("Expected value, but found operator: " + token);

                prev = tokens::Operator;
                needNext = tokens::Value;
            }

            AddOperator(output, operators, oper);

            if (oper.Type() == OperType::binary || oper.Pos() == OperPos::prefix)
                isPrevOperator = true;
        }
        else { // token is value
            output.push_back(std::make_unique<Value>(float100(token)));
            isPrevOperator = false;
            prev = tokens::Value;
            needNext = tokens::None;
        }

        iterPrev = iterNext + 1;
        iterNext = std::find(iterPrev, expression.end(), ' ');
    }

    if (needNext != tokens::None)
        throw std::logic_error("Expected more values after operator " + operators.back().getOperator());

    //Adding remaining operators
    while (!operators.empty())
    {
        if (operators.back().Type() == OperType::openBracket) {
            throw std::logic_error("Bracket mismatch: missing close bracket");
        }
        else {
            output.push_back(std::make_unique<Operator>(operators.back()));
            operators.pop_back();
        }
    }

    return output;
}

//float100 MathExpression::Calculate(std::list<std::unique_ptr<Token>> output)
float100 MathExpression::Calculate(std::string expression)
{
    std::list<std::unique_ptr<Token>> output = Parse(expression);

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
        auto Decrement = [&output](std::list<std::unique_ptr<Token>>::iterator& iter)
        {
            if (iter == output.begin())
                throw std::logic_error("Not enough values for operation");
            return --iter;
        };
        std::list<std::unique_ptr<Token>>::iterator tmp = oper;

        std::list<std::unique_ptr<Token>>::iterator secondValue = output.end();

        //if operator is binary
        if (static_cast<Operator*>(oper->get())->Type() == OperType::binary)
            secondValue = Decrement(tmp); // than we have a second value

        std::list<std::unique_ptr<Token>>::iterator firstValue = Decrement(tmp);

        //if first value or second value are not really a values
        if ((*firstValue)->isOperator() && secondValue != output.end() && (*secondValue)->isOperator())
            throw std::logic_error("Wrong sequence!");

        std::unique_ptr<Token> result;

        if (secondValue != output.end())
            result = std::make_unique<Value>(DoOperation(*firstValue, *secondValue, *oper));
        else
            result = std::make_unique<Value>(DoOperation(*firstValue, *oper));

        firstValue->swap(result);
        output.erase(oper);

        if (secondValue != output.end())
            output.erase(secondValue);

    }

    return ((Value*)output.front().get())->getValue();
}

//for binary operators
Value MathExpression::DoOperation(std::unique_ptr<Token>& firstValue,
                                  std::unique_ptr<Token>& secondValue,
                                  std::unique_ptr<Token>& _oper)
{
    const float100& fValue = static_cast<Value*>(firstValue.get())->getValue();
    const float100& sValue = static_cast<Value*>(secondValue.get())->getValue();
    const std::string& oper = static_cast<Operator*>(_oper.get())->getOperator();

    if (!binaryOperators.contains(oper))
        throw std::logic_error("Wrong operator: " + oper);

    return binaryOperators.at(oper).functor(fValue, sValue);
}

//for unary operators
Value MathExpression::DoOperation(std::unique_ptr<Token>& _value,
                                  std::unique_ptr<Token>& _oper)
{
    const float100& value = static_cast<Value*>(_value.get())->getValue();
    const std::string& oper = static_cast<Operator*>(_oper.get())->getOperator();

    if (!unaryOperators.contains(oper))
        throw std::logic_error("Wrong operator: " + oper);

    return unaryOperators.at(oper).functor(value);
}

void MathExpression::AddOperator(std::list<std::unique_ptr<Token>>& output,
                                 std::list<Operator>& operators, Operator oper)
{
    if (oper.Type() == OperType::closeBracket) {
        while (true) { //pull out operator until we find open bracket
            if (!operators.empty()) {
                if (operators.back().Type() != OperType::openBracket)
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
            && oper.Type() != OperType::openBracket)
        {
            output.push_back(std::make_unique<Operator>(operators.back()));
            operators.pop_back();
        }
        operators.push_back(oper);
    }
}