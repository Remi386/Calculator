#pragma once
#include <string>
#include <list>
#include <stdexcept>
#include <memory>
#include <boost/multiprecision/cpp_dec_float.hpp>

using float100 = boost::multiprecision::cpp_dec_float_100;

class Token
{
public:
	virtual ~Token() = default;

	virtual bool isOperator() = 0;
};

class Operator : public Token
{
public:

	Operator(std::string s)
		: oper(s)
	{}

	virtual bool isOperator() override
	{
		return true;
	};

	int getPriority()
	{
		if (oper[0] == '(')
			return -1;

		if (oper[0] == '+' || oper[0] == '-')
			return 0;

		if (oper[0] == '*' || oper[0] == '/')
			return 1;

		if (oper[0] == '^')
			return 2;

		throw std::logic_error("Wrong operator: " + oper);
	}

	const std::string& getOperator()
	{
		return oper;
	}

private:
	std::string oper;
};

class Value : public Token
{
public:
	Value(float100 _value)
		:value(_value)
	{}

	const float100& getValue()
	{
		return value;
	}

	virtual bool isOperator() override
	{
		return false;
	};

private:
	float100 value;
};

int HandleOperator(std::string& str, size_t offset);

int HandleBracket(std::string& str, size_t offset);

void AddSpaces(std::string& str);

void AppropriateView(std::string& str);

bool IsOperator(std::string& str);

std::list<std::unique_ptr<Token>> Parse(std::string& expression);

float100 Calculate(std::list<std::unique_ptr<Token>> output);

void AddOperator(std::list<std::unique_ptr<Token>>& output,
	std::list<Operator>& operators, Operator oper);

Value DoOperation(std::unique_ptr<Token>& firstValue,
				  std::unique_ptr<Token>& secondValue,
				  std::unique_ptr<Token>& _oper);