#define BOOST_TEST_MODULE MathExpressionTest
#include <boost/test/unit_test.hpp>
#include "MathExpression.h"

constexpr double precision = 0.00001;

BOOST_AUTO_TEST_CASE(AppropriateViewTest)
{
	//plus multiply brackets
	std::string test0 = "1+ ( 1* 2)  ";
	AppropriateView(test0);
	BOOST_CHECK(test0 == std::string("1 + ( 1 * 2 ) "));

	//power + division
	std::string test1 = "      (1^(1/2))   ";
	AppropriateView(test1);
	BOOST_CHECK(test1 == std::string("( 1 ^ ( 1 / 2 ) ) "));

	//minus
	std::string test2 = "      (1-7/9)   ";
	AppropriateView(test2);
	BOOST_CHECK(test2 == std::string("( 1 - 7 / 9 ) "));

	//power
	std::string test3 = " 1.6 ^ 43.9   ";
	AppropriateView(test3);
	BOOST_CHECK(test3 == std::string("1.6 ^ 43.9 "));
	
	//float point
	std::string test4 = "(9.7 * 1.5) - (1.3 + 2)";
	AppropriateView(test4);
	BOOST_CHECK(test4 == std::string("( 9.7 * 1.5 ) - ( 1.3 + 2 ) "));

	//factorial
	std::string test5 = "!1 * sin(43.9)   ";
	AppropriateView(test5);
	BOOST_CHECK(test5 == std::string("! 1 * sin ( 43.9 ) "));

	//sin + cos
	std::string test6 = "sin1.6 - cos 43.9   ";
	AppropriateView(test6);
	BOOST_CHECK(test6 == std::string("sin 1.6 - cos 43.9 "));

	//tan
	std::string test7 = "tan7";
	AppropriateView(test7);
	BOOST_CHECK(test7 == std::string("tan 7 "));

	//sqrt
	std::string test8 = " sqrt9 - cos2 + sin1   ";
	AppropriateView(test8);
	BOOST_CHECK(test8 == std::string("sqrt 9 - cos 2 + sin 1 "));

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

	std::string expression6 = "5.6 + 4.4 * 2 / (5.1 - 1.7)^2.7";
	BOOST_CHECK_CLOSE(Calculate(Parse(expression6)), 5.92321357741, precision);

	std::string expression7 = "5!";
	BOOST_CHECK_CLOSE(Calculate(Parse(expression7)), 120., precision);
	
	std::string expression8 = "3.5!";
	BOOST_CHECK_CLOSE(Calculate(Parse(expression8)), 11.6317283, precision);

	std::string expression9 = "sqrt81";
	BOOST_CHECK_CLOSE(Calculate(Parse(expression9)), 9., precision);

	std::string expression10 = "sqrt37";
	BOOST_CHECK_CLOSE(Calculate(Parse(expression10)), 6.0827625, precision);

	std::string expression11 = "sin8.5";
	BOOST_CHECK_CLOSE(Calculate(Parse(expression11)), 0.7984871, precision);

	std::string expression12 = "cos4.1";
	BOOST_CHECK_CLOSE(Calculate(Parse(expression12)), -0.57482394, precision);

	std::string expression13 = "tan3.9";
	BOOST_CHECK_CLOSE(Calculate(Parse(expression13)), 0.94742464, precision);

	std::string expression14 = "log100";
	BOOST_CHECK_CLOSE(Calculate(Parse(expression14)), 2., precision);

	std::string expression15 = "log 3.9";
	BOOST_CHECK_CLOSE(Calculate(Parse(expression15)), 0.59106460, precision);

	std::string expression16 = "ln5.2";
	BOOST_CHECK_CLOSE(Calculate(Parse(expression16)), 1.64865862, precision);

	std::string expression17 = " (2^7 - 9";
	BOOST_CHECK_THROW(Calculate(Parse(expression17)), std::logic_error);

	std::string expression18 = " 2^7) - 9";
	BOOST_CHECK_THROW(Calculate(Parse(expression18)), std::logic_error);

	std::string expression19 = "sin 4 1";
	BOOST_CHECK_THROW(Calculate(Parse(expression19)), std::logic_error);

	std::string expression20 = "1 - + 2";
	BOOST_CHECK_THROW(Calculate(Parse(expression20)), std::logic_error);

	std::string expression21 = "8 + - 3";
	BOOST_CHECK_CLOSE(Calculate(Parse(expression21)), 5., precision);

	std::string expression22 = "-8 + (-3 * 4) - 6 ^ -4";
	BOOST_CHECK_CLOSE(Calculate(Parse(expression22)), -20.0007716, precision);

	std::string expression23 = "+ 1 - + 2";
	BOOST_CHECK_THROW(Parse(expression23), std::logic_error);

	std::string expression24 = "1 + 2 +";
	BOOST_CHECK_THROW(Parse(expression24), std::logic_error);

	std::string expression25 = "!1";
	BOOST_CHECK_THROW(Parse(expression25), std::logic_error);

	std::string expression26 = "1! sin 2";
	BOOST_CHECK_THROW(Parse(expression26), std::logic_error);

	std::string expression27 = "sin1 cos2";
	BOOST_CHECK_THROW(Parse(expression27), std::logic_error);

	std::string expression28 = "5! + log(10)";
	BOOST_CHECK_CLOSE(Calculate(Parse(expression28)), 121, precision);
}