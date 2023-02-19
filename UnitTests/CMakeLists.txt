add_executable(CalculatorTest
			   MExpUnitTests.cpp)
			   
add_dependencies(CalculatorTest Calculator)

target_link_libraries(CalculatorTest 
					  PRIVATE
					  MathExpressionLibrary
					  Boost::unit_test_framework)
					   
add_test(NAME testCalculator
		 COMMAND $<TARGET_FILE:CalculatorTest>)