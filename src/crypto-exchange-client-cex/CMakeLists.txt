﻿#
cmake_minimum_required (VERSION 3.20)


#
add_library (${PROJECT_NAME} 
	src/client.cpp
	src/wsMessage.cpp
)


#
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 23)
