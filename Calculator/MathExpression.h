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
	enum class OperatorType {
		unary,
		binary,
		openBracket,
		closeBracket
	};

	enum class Position {
		prefix,
		postfix,
		nonUnary
	};

	Operator(std::string s, OperatorType _type);

	virtual bool isOperator() override
	{
		return true;
	};

	OperatorType Type()
	{
		return operType;
	}

	Position Pos()
	{
		return position;
	}

	int getPriority()
	{
		return priority;
	}

	const std::string& getOperator()
	{
		return oper;
	}

private:
	std::string oper;
	OperatorType operType;
	Position position = Position::nonUnary;
	int priority = -10;
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

size_t getOperatorLength(const std::string& str, size_t offset);

int HandleToken(std::string& str, size_t offset, size_t length);

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

Value DoOperation(std::unique_ptr<Token>& _value,
				  std::unique_ptr<Token>& _oper);

Operator MakeOperator(const std::string& str, bool isPrevOperator);