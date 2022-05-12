#include "MathExpression.h"
#include <regex>
#include <vector>

const std::regex operators("(log|ln|sqrt|sin|cos|tan|[-*/+)(!^])");
const std::regex binaryOperators("[-*/+^]");
const std::regex unaryOperators("(log|ln|sqrt|sin|cos|tan|!)");

Operator::Operator(std::string s, OperatorType _type)
    : oper(s), operType(_type)
{
    if (operType == OperatorType::binary) {

        if (oper.compare("+") == 0 || oper.compare("-") == 0)
            priority = 0;
        else if (oper.compare("*") == 0 || oper.compare("/") == 0)
            priority = 1;
        else if (oper.compare("^") == 0) 
            priority = 2;
    }
    else if(operType == OperatorType::unary) {

        if (oper.compare("!") == 0) {
            priority = 3;
            position = Position::postfix;
        }
        else if (oper.compare("sin") == 0 || oper.compare("cos") == 0 ||
                 oper.compare("tan") == 0 || oper.compare("sqrt") == 0 ||
                 oper.compare("log") == 0 || oper.compare("ln") == 0)
        {
            priority = 3;
            position = Position::prefix;
        }
        else if (oper.compare("-") == 0) {
            priority = 4;
            position = Position::prefix;
        }
    }
    else { //brackets
        priority = -1;
    }

    if(priority == -10) //if priority has default value
        throw std::logic_error("Wrong operator: " + oper);
}

Operator MakeOperator(const std::string& str, bool isPrevOperator)
{
    //unary minus
    if (isPrevOperator && str.compare("-") == 0)
        return Operator(str, Operator::OperatorType::unary);

    if (str.compare("(") == 0)
        return Operator(str, Operator::OperatorType::openBracket);
    
    if (str.compare(")") == 0)
        return Operator(str, Operator::OperatorType::closeBracket);

    if (std::regex_match(str, binaryOperators)) 
        return Operator(str, Operator::OperatorType::binary);

    if (std::regex_match(str, unaryOperators))
        return Operator(str, Operator::OperatorType::unary);

    throw std::logic_error("Wrong operator: " + str);
}

size_t getOperatorLength(const std::string& str, size_t offset)
{
    if (str.substr(offset, 2) == "ln")
        return 1;

    if (str.substr(offset, 3) == "cos" || str.substr(offset, 3) == "sin" ||
        str.substr(offset, 3) == "tan" || str.substr(offset, 3) == "log")
        return 2;

    if (str.substr(offset, 4) == "sqrt")
        return 3;

    return 0;
}

int HandleToken(std::string& str, size_t offset, size_t length)
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
void AddSpaces(std::string& str)
{
    //we cant insert spaces and use regex iterators in the same string
    std::string copy(str);

    std::sregex_iterator oper_begin(str.begin(), str.end(), operators);
    std::sregex_iterator oper_end = std::sregex_iterator();

    //counting how many spaces we inserted to calculate offset
    int spacesCount = 0;

    while (oper_begin != oper_end)
    {
        std::smatch match = *oper_begin;

        size_t offset = match.position();
        size_t length = getOperatorLength(str, offset);

        spacesCount += HandleToken(copy, offset + spacesCount, length);

        ++oper_begin;
    }
    str = copy;
}

void AppropriateView(std::string& str)
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

    //deleting spaces in the beginning
    while (str[0] == ' ')
        str.erase(0, 1);

    str.shrink_to_fit();
}

bool IsOperator(std::string& str)
{
    if (str.size() > 4 || str.empty())
        return false;

    return std::regex_search(str, operators);
}

std::list<std::unique_ptr<Token>> Parse(std::string& expression)
{
    AppropriateView(expression);

    std::list<std::unique_ptr<Token>> output;
    std::list<Operator> operators;

    if (expression.empty())
        output.push_back(std::make_unique<Value>(0));

    using opType = Operator::OperatorType;

    enum class tokens {
        Operator,
        Value,
        Nothing
    } prev = tokens::Nothing, needNext = tokens::Nothing;

    bool isPrevOperator = true;

    auto iterPrev = expression.begin();
    auto iterNext = std::find(expression.begin(), expression.end(), ' ');

    while (iterNext != expression.end())
    {
        std::string token(iterPrev, iterNext);
        if (IsOperator(token)) {
            Operator oper = MakeOperator(token, isPrevOperator);

            //Checking an expression for correctness in infis notation
            if (oper.Type() == opType::unary) {
                if (oper.Pos() == Operator::Position::postfix) {
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
            else if(oper.Type() == opType::binary) {

                if(prev == tokens::Operator)
                    throw std::logic_error("Expected value, but found operator: " + token);

                prev = tokens::Operator;
                needNext = tokens::Value;
            }

            AddOperator(output, operators, oper);

            if(oper.Type() == opType::binary || oper.Pos() == Operator::Position::prefix)
                    isPrevOperator = true;
        }
        else { // token is value
            output.push_back(std::make_unique<Value>(float100(token)));
            isPrevOperator = false;
            prev = tokens::Value;
            needNext = tokens::Nothing;
        }

        iterPrev = iterNext + 1;
        iterNext = std::find(iterPrev, expression.end(), ' ');
    }

    if (needNext != tokens::Nothing)
        throw std::logic_error("Expected more values after operator " + operators.back().getOperator());

    //Adding remaining operators
    while (!operators.empty())
    {
        if (operators.back().Type() != opType::openBracket) {
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
        auto Decrement = [&output](std::list<std::unique_ptr<Token>>::iterator& iter)
        {
            if (iter == output.begin())
                throw std::logic_error("Not enough values for operation");
            return --iter;
        };
        std::list<std::unique_ptr<Token>>::iterator tmp = oper;

        std::list<std::unique_ptr<Token>>::iterator secondValue = output.end();

        //if operator is binary
        if (static_cast<Operator*>(oper->get())->Type() == Operator::OperatorType::binary)
            secondValue = Decrement(tmp); // than we have a second value

        std::list<std::unique_ptr<Token>>::iterator firstValue = Decrement(tmp);

        //if first value or second value are not really a values
        if((*firstValue)->isOperator() && secondValue != output.end() && (*secondValue)->isOperator())
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
Value DoOperation(std::unique_ptr<Token>& firstValue,
                  std::unique_ptr<Token>& secondValue,
                  std::unique_ptr<Token>& _oper)
{
    auto& fValue = static_cast<Value*>(firstValue.get())->getValue();
    auto& sValue = static_cast<Value*>(secondValue.get())->getValue();
    auto& oper = static_cast<Operator*>(_oper.get())->getOperator();

    if (oper.compare("+") == 0)
        return Value(fValue + sValue);

    if (oper.compare("-") == 0)
        return Value(fValue - sValue);

    if (oper.compare("*") == 0)
        return Value(fValue * sValue);

    if (oper.compare("/") == 0)
        return Value(fValue / sValue);

    if (oper.compare("^") == 0)
        return Value(boost::multiprecision::pow(fValue, sValue));

    throw std::logic_error("Wrong operator: " + oper);
}

//for unary operators
Value DoOperation(std::unique_ptr<Token>& _value,
                  std::unique_ptr<Token>& _oper)
{
    auto& value = static_cast<Value*>(_value.get())->getValue();
    auto& oper = static_cast<Operator*>(_oper.get())->getOperator();

    if (oper.compare("!") == 0)
        return Value(boost::math::tgamma(value + 1));

    if (oper.compare("sin") == 0)
        return Value(boost::multiprecision::sin(value));
    
    if (oper.compare("cos") == 0)
        return Value(boost::multiprecision::cos(value));

    if (oper.compare("tan") == 0)
        return Value(boost::multiprecision::tan(value));

    if (oper.compare("sqrt") == 0)
        return Value(boost::multiprecision::sqrt(value));

    if (oper.compare("log") == 0)
        return Value(boost::multiprecision::log10(value));

    if (oper.compare("ln") == 0)
        return Value(boost::multiprecision::log(value));

    if (oper.compare("-") == 0)
        return Value(-value);

    throw std::logic_error("Wrong operator: " + oper);
}

void AddOperator(std::list<std::unique_ptr<Token>>& output,
                 std::list<Operator>& operators, Operator oper)
{
    if (oper.Type() == Operator::OperatorType::closeBracket) {
        while (true) { //pull out operator until we find open bracket
            if (!operators.empty()) {
                if (operators.back().Type() != Operator::OperatorType::openBracket)
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
            && oper.Type() != Operator::OperatorType::openBracket)
        {
            output.push_back(std::make_unique<Operator>(operators.back()));
            operators.pop_back();
        }
        operators.push_back(oper);
    }
}