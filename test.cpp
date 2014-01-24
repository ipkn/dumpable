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

#define ASSERT_NOT_EQUAL(a,b) if (a == b) \
    { \
        ostringstream os; \
        os << "Assert error: not expected " #b " (" << b << ") but " #a " (" << a << ")"; \
        fail(os.str()); \
    }

#define TEST(x) \
    void test_##x##_();\
    struct TestAppendHelper ## x {TestAppendHelper ## x(){allTests.push_back(pair<string,function<void()>>(#x,test_##x##_));}} test_append_helper_## x; \
    void test_##x##_()

#define IGNORED_TEST(x) \
    void test_##x##_()

TEST(simple_example)
{
    // example data structure

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

    // building sample data

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

    // Dump to stringstream
    // NOTE: No need to write serialization code for struct student and struct classroom.

    ostringstream os;
    dumpable::write(c, os);

    // Destroy the contents of sample data.

    c.students.clear();
    c.class_name = "invalid";

    // read from dumped data
    // NOTE: from_dumped_buffer is just a constant time operation! 
    //   (see implementation in dumpable.h)
    // You can even use a pointer from mmap.

    string buffer = os.str();
    const classroom* c2 = dumpable::from_dumped_buffer<classroom>(buffer.data());

    // Check contents of c2
    // You should keep the buffer given to from_dumped_buffer function.
    // These data are only valid when the buffer is valid.

    ASSERT_EQUAL("1001", c2->class_name);
    ASSERT_EQUAL(L"Alice", c2->students[0].name);
    ASSERT_EQUAL(2, c2->students[0].score);
    ASSERT_EQUAL(L"Bob", c2->students[1].name);
    ASSERT_EQUAL(5, c2->students[1].score);
    ASSERT_EQUAL(L"\ud55c\uae00", c2->students[2].name);
    ASSERT_EQUAL(13, c2->students[2].score);
}

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
    const node* p = &n1;
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
    p = dumpable::from_dumped_buffer<node>(buffer.data());

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

TEST(vector)
{
    vector<int> v;
    dvector<int> v2, v3;
    v.push_back(100);
    v.push_back(300);
    v.push_back(200);
    v2 = v;
    v2 = v2;
    v3 = v2;
    ASSERT_EQUAL(3, v3.size());
    ASSERT_EQUAL(300, v3[1]);
    v3.clear();
    ASSERT_EQUAL(0, v3.size());
    v2 = std::move(v2);
    ASSERT_EQUAL(3, v2.size());
    v3 = std::move(v2);
    ASSERT_EQUAL(3, v3.size());
    ASSERT_EQUAL(0, v2.size());
    ASSERT_EQUAL(300, v3[1]);
}

TEST(string)
{
    dstring dumpable_empty("");
    string stl_empty("");

    ASSERT_EQUAL(dumpable_empty, stl_empty);
    ASSERT_NOT_EQUAL(dumpable_empty, "Hello World");
    ASSERT_EQUAL(dumpable_empty, "");
    ASSERT_NOT_EQUAL("Hello World", dumpable_empty);
    ASSERT_EQUAL("", dumpable_empty);

    dstring dumpable_abc("abc");
    dstring dumpable_abc2("abc");
    string stl_abc("abc");
    dstring dumpable_def("def");
    string stl_def("def");

    ASSERT_EQUAL(dumpable_abc,dumpable_abc);
    dumpable_abc = dumpable_abc;
    ASSERT_EQUAL(dumpable_abc,dumpable_abc);
    dumpable_abc = std::move(dumpable_abc);
    ASSERT_EQUAL(dumpable_abc,dumpable_abc);
    ASSERT_EQUAL(dumpable_abc,dstring("abc"));
    ASSERT_NOT_EQUAL(dumpable_abc,dstring("abcd"));
    ASSERT_EQUAL(dumpable_abc,dumpable_abc2);
    ASSERT_EQUAL(dumpable_abc,stl_abc);
    ASSERT_EQUAL(dumpable_def,stl_def);
    ASSERT_EQUAL(dumpable_abc2, dumpable_abc);
    ASSERT_EQUAL(stl_abc, dumpable_abc);
    ASSERT_EQUAL(stl_def, dumpable_def);

    ASSERT_NOT_EQUAL(dumpable_abc,stl_def);
    ASSERT_NOT_EQUAL(dumpable_def,stl_abc);

    ASSERT_NOT_EQUAL(dumpable_abc,string("abcd"));
    ASSERT_NOT_EQUAL(string("efgh"),dumpable_def);

    dstring moved_abc(std::move(dumpable_abc2));
    ASSERT_EQUAL(moved_abc, dumpable_abc);
    ASSERT_EQUAL(0, dumpable_abc2.size());

    dstring moved_abc2;
    moved_abc2 = std::move(moved_abc);
    ASSERT_EQUAL(moved_abc2, dumpable_abc);
    ASSERT_EQUAL(0, moved_abc.size());

    string testAppend;
    for(auto it = dumpable_abc.begin(); it != dumpable_abc.end(); ++it)
    {
        testAppend += *it;
    }
    ASSERT_EQUAL(testAppend, dumpable_abc);
    ASSERT_EQUAL('a', dumpable_abc.front());
    ASSERT_EQUAL('c', dumpable_abc.back());

    ostringstream os;
    os << dumpable_abc;
    ASSERT_EQUAL(os.str(), dumpable_abc);
    ASSERT_EQUAL(os.str().c_str(), dumpable_abc);
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
    const decltype(data)* p = dumpable::from_dumped_buffer<const decltype(data)>(buffer.data());

    
    {
        const dmap<int, dstring>& data = *p;
        ASSERT_EQUAL("one", data.find(1)->second);
        ASSERT_EQUAL("two", data.find(2)->second);
        ASSERT_EQUAL("five", data.find(5)->second);

        ASSERT_EQUAL(0, data.count(3));
        ASSERT_EQUAL(1, data.count(1));
        ASSERT_EQUAL(3, data.size());
    }

}

TEST(basic_implementation)
{
    struct embedded_empty
    {
        // intentionally empty
    };
    struct embedded 
    {
        int x;
        dptr<embedded_empty> y;
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
    d.b->y = new embedded_empty;
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
