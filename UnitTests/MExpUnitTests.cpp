#define BOOST_TEST_MODULE MathExpressionTest
#include <boost/test/unit_test.hpp>
#include "MathExpression.h"

constexpr double precision = 0.00001;

BOOST_AUTO_TEST_CASE(AppropriateViewTest)
{
	std::string test0 = "1+ ( 1* 2)  ";
	AppropriateView(test0);
	BOOST_CHECK(test0 == std::string("1 + ( 1 * 2 ) "));

	std::string test1 = "      (1^(1/2))   ";
	AppropriateView(test1);
	BOOST_CHECK(test1 == std::string("( 1 ^ ( 1 / 2 ) ) "));

	std::string test2 = "      (1-7/9)   ";
	AppropriateView(test2);
	BOOST_CHECK(test2 == std::string("( 1 - 7 / 9 ) "));

	std::string test3 = "+ 1 2";
	BOOST_CHECK_THROW(AppropriateView(test3), std::logic_error);
}

BOOST_AUTO_TEST_CASE(CalculateTest)
{
	std::string expression0 = "1 + 1";
	BOOST_CHECK_CLOSE(Calculate(Parse(expression0)), 2., precision);

	std::string expression1 = "5/2";
	BOOST_CHECK_CLOSE(Calculate(Parse(expression1)), 2.5, precision);

	std::string expression2 = "4*2-1";
	BOOST_CHECK_CLOSE(Calculate(Parse(expression2)), 7., precision);

	std::string expression3 = "4^3 * 2^7 - 9";
	BOOST_CHECK_CLOSE(Calculate(Parse(expression3)), 8183., precision);

	std::string expression4 = "5 * (4 + 7) - (12 ^ 2)";
	BOOST_CHECK_CLOSE(Calculate(Parse(expression4)), -89., precision);

	std::string expression5 = "3 + 4 * 2 / (1 - 5)^2";
	BOOST_CHECK_CLOSE(Calculate(Parse(expression5)), 3.5, precision);

	std::string expression6 = " - 4^3 * 2^7 - 9";
	BOOST_CHECK_THROW(Calculate(Parse(expression6)), std::logic_error);

	std::string expression7 = " (2^7 - 9";
	BOOST_CHECK_THROW(Calculate(Parse(expression7)), std::logic_error);

	std::string expression8 = " 2^7) - 9";
	BOOST_CHECK_THROW(Calculate(Parse(expression8)), std::logic_error);

	std::string expression9 = "5 + -2";
	BOOST_CHECK_THROW(Calculate(Parse(expression9)), std::logic_error);
}