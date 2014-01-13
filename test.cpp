#include <vector>
#include <sstream>
#include <string>
#include <iostream>
#include <functional>

#include "dumpable.h"

using namespace std;
using namespace dumpable;

string currentTestName;
vector<pair<string,function<void()>>> allTests;

bool failed = false;

void setup()
{
    failed = false;
}

void fail(string msg)
{
    failed = true;
    cerr << "TEST FAIL" << endl;;
    cerr << msg << endl;
}

#define ASSERT_EQUAL(a,b) if (a != b) \
    { \
        ostringstream os; \
        /*os << "Assert error: expected " #b " (" << b << ") but " #a " (" << a << ")";*/ \
        os << "Assert error: expected " #b "  but " #a ; \
        fail(os.str()); \
    }
#define TEST(x) \
    void test_##x##_();\
    struct TestAppendHelper ## x {TestAppendHelper ## x(){allTests.push_back({#x,test_##x##_});}} test_append_helper_## x; \
    void test_##x##_()

#define IGNORED_TEST(x) \
    void test_##x##_()

TEST(dptr_impl)
{
    struct node
    {
        int d;
        dptr<node> n;
    };
    node n1, n2, n3;
    n1.d = 1;
    n1.n = &n2;
    n2.d = 2;
    n2.n = &n3;
    n3.d = 3;
    ASSERT_EQUAL(nullptr, n3.n);

    int sum = 0;
    node* p = &n1;
    ASSERT_EQUAL(1, p->d);
    ASSERT_EQUAL(&n2, p->n);
    while(p)
    {
        sum += p->d;
        p = p->n;
    }
    ASSERT_EQUAL(6, sum);
}

IGNORED_TEST(basic_implementation)
{
    struct embedded 
    {
        int x;
    };
    struct data
    {
        int a;
        dptr<embedded> b;
        int c;
    };
    data d;
    d.a = 1;
    d.b = new embedded;
    d.b->x = 2;
    d.c = 3;

    //ostringstream os;
    //dumpable::write(os);

    /*string buffer = os.str();

    dumpable::dpool::set_reading();
    dumpable::dpool::set_pool(&buffer[0], buffer.size());
    data* e = (data*)&buffer[0];
    ASSERT_EQUAL(1, e->a);
    ASSERT_EQUAL(2, e->b->x);
    ASSERT_EQUAL(3, e->c);*/
}

int testmain()
{
    bool isAnyTestFailed = false;
    for(auto it = allTests.begin(); it != allTests.end(); ++it)
    {
        setup();
        currentTestName = it->first;
        it->second();
        if (failed)
        {
            isAnyTestFailed = true;
            cerr << "F";
        }
        else
        {
            cerr << ".";
        }
    }
    cerr << endl;
    return isAnyTestFailed ? -1 : 0;
}

int main()
{
    return testmain();
}
