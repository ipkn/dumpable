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
        os << "Assert error: expected " #b " (" << b << ") but " #a " (" << a << ")"; \
        fail(os.str()); \
    }

#define TEST(x) \
    void test_##x##_();\
    struct TestAppendHelper ## x {TestAppendHelper ## x(){allTests.push_back(pair<string,function<void()>>(#x,test_##x##_));}} test_append_helper_## x; \
    void test_##x##_()

#define IGNORED_TEST(x) \
    void test_##x##_()

TEST(linked_list)
{
    struct node
    {
        int d;
        dptr<node> n;
    };

    // building sample linked list

    node n1, n2, n3;
    n1.d = 1;
    n1.n = &n2;
    n2.d = 2;
    n2.n = &n3;
    n3.d = 3;
    ASSERT_EQUAL(0, n3.n);

    // dptr<node> can be used as node*

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

    // dump linked list to stringstream

    ostringstream os;
    dumpable::write(n1, os);

    // destroy original data
    n3.d = n2.d = n1.d = -100;
    n3.n = n2.n = n1.n = nullptr;

    // read from dumped data

    string buffer = os.str();
    p = dumpable::from_dumped_buffer<node>(&buffer[0]);

    // recheck data

    sum = 0;
    ASSERT_EQUAL(1, p->d);
    while(p)
    {
        sum += p->d;
        p = p->n;
    }
    ASSERT_EQUAL(6, sum);
}

TEST(map)
{
    map<int, dstring> original;
    original.insert(make_pair(1, "one"));
    original.insert(make_pair(2, "two"));
    original.insert(make_pair(5, "five"));

    dmap<int, dstring> data2(original);
    dmap<int, dstring> data = std::move(data2);

    ASSERT_EQUAL(0, data2.size());
    ASSERT_EQUAL(0, data2.count(1));

    ASSERT_EQUAL("one", data.find(1)->second);
    ASSERT_EQUAL("two", data.find(2)->second);
    ASSERT_EQUAL("five", data.find(5)->second);

    ASSERT_EQUAL(0, data.count(3));
    ASSERT_EQUAL(1, data.count(1));
    ASSERT_EQUAL(3, data.size());

    ostringstream os;
    dumpable::write(data, os);

    data.clear();
    ASSERT_EQUAL(0, data.count(1));
    ASSERT_EQUAL(0, data.count(2));
    ASSERT_EQUAL(0, data.count(5));
    ASSERT_EQUAL(0, data.size());


    string buffer = os.str();
    decltype(data)* p = dumpable::from_dumped_buffer<decltype(data)>(&buffer[0]);

    
    {
        dmap<int, dstring>& data = *p;
        ASSERT_EQUAL("one", data.find(1)->second);
        ASSERT_EQUAL("two", data.find(2)->second);
        ASSERT_EQUAL("five", data.find(5)->second);

        ASSERT_EQUAL(0, data.count(3));
        ASSERT_EQUAL(1, data.count(1));
        ASSERT_EQUAL(3, data.size());
    }

}

TEST(vector_and_string)
{
    struct student
    {
        dwstring name;
        int score;
        student(){}
        student(const wstring& name, int score) : name(name), score(score) {}
    };
    struct classroom
    {
        dstring class_name;
        dvector<student> students;
    };

    vector<student> testData;
    testData.push_back(student(L"Alice",2));
    testData.push_back(student(L"Bob",5));
    testData.push_back(student(L"\ud55c\uae00",13));
    classroom c;
    c.class_name = "1001";
    c.students = testData;
    {
        dvector<student> move_test(testData);
        c.students = std::move(move_test);
    }

    ostringstream os;
    dumpable::write(c, os);

    c.students.clear();

    string buffer = os.str();
    classroom* c2 = dumpable::from_dumped_buffer<classroom>(&buffer[0]);

    ASSERT_EQUAL("1001", c2->class_name);
    ASSERT_EQUAL(L"Alice", c2->students[0].name);
    ASSERT_EQUAL(2, c2->students[0].score);
    ASSERT_EQUAL(L"Bob", c2->students[1].name);
    ASSERT_EQUAL(5, c2->students[1].score);
    ASSERT_EQUAL(L"\ud55c\uae00", c2->students[2].name);
    ASSERT_EQUAL(13, c2->students[2].score);

    classroom x = std::move(*c2);
    ASSERT_EQUAL("1001", x.class_name);
    ASSERT_EQUAL(L"Alice", x.students[0].name);
    ASSERT_EQUAL(2, x.students[0].score);
    ASSERT_EQUAL(L"Bob", x.students[1].name);
    ASSERT_EQUAL(5, x.students[1].score);
    ASSERT_EQUAL(L"\ud55c\uae00", x.students[2].name);
    ASSERT_EQUAL(13, x.students[2].score);
}

TEST(basic_implementation)
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

    ostringstream os;
    dumpable::write(d, os);

    string buffer = os.str();
    data* e = dumpable::from_dumped_buffer<data>(&buffer[0]);

    ASSERT_EQUAL(1, e->a);
    ASSERT_EQUAL(2, e->b->x);
    ASSERT_EQUAL(3, e->c);
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
