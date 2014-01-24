all: test
test: test.cpp dptr.h dumpable.h dpool.h dvector.h dstring.h dmap.h
	g++ -Wall -std=c++11 -g --coverage -otest test.cpp 
	./test
	gcov -r ./test


