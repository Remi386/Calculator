add_library(MathExpressionLibrary
			OBJECT
			MathExpression.cpp
			MathExpression.h)

target_link_libraries(MathExpressionLibrary
					  PUBLIC
					  Boost::headers)

target_include_directories(MathExpressionLibrary PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
			
find_package(Qt6 6.2.0 REQUIRED COMPONENTS Core Gui Widgets)

add_executable(Calculator
			   main.cpp
			   Calculator.cpp
			   Calculator.h
			   Calculator.ui
			   Calculator.qrc
			   Calculator.rc)

target_link_libraries(Calculator 
					  PRIVATE 
					  MathExpressionLibrary
					  Qt::Core
					  Qt::Widgets)
					  
if(DEFINED WIN32)
	include(../cmake/deploy_util.cmake)
	WinDeployApplication(Calculator)
endif()