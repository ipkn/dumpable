all: test
test: test.cpp dptr.h dumpable.h dpool.h dvector.h dstring.h dmap.h
	g++ -std=c++11 -g -otest test.cpp 
