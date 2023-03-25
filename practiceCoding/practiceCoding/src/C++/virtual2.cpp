#include <iostream>
using namespace std;

class Base {
public:
    Base() {}
    virtual void fun() { cout << "Base" << endl; }
};

class D :public Base
{
public:
    D() :val(0) {}
    void fun() { cout << "D" << endl; }

    int val;
};

int main()
{
    D d;
    Base* base1 = &d;
    Base base2 = d;
    Base& base3 = d;

    cout << "d:    " << &d << ",size:" << sizeof(d) << endl;
    cout << "base1:" << base1 << ",size:" << sizeof(*base1) << endl;
    cout << "base2:" << &base2 << ",size:" << sizeof(base2) << endl;
    cout << "base3:" << &base3 << ",size:" << sizeof(base3) << endl;

    d.fun();
    base1->fun();
    base2.fun();
    base3.fun();
   
    return 0;
}