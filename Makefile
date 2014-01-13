all: test
test: test.cpp dptr.h
	g++ -std=c++11 -g -otest test.cpp 
