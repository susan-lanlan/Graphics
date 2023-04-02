#include<iostream>
#include<memory>

using namespace std;

class demo {
public:
    string a;
    int b;
    demo(const char* ain, int bin) :a(ain),b(bin) {
    }
    friend ostream& operator<<(ostream& os, const demo& d);
};
ostream& operator<<(ostream& os, const demo& d) {
    cout << "demo.a: " << d.a << endl;
    cout << "demo.b: " << d.b << endl;
    return os;
}

int main() {
    cout << "hello vscode!" << endl;
    demo* p = new demo("hello",1);
    unique_ptr<demo> ps1, ps2;
    ps1 = unique_ptr<demo>(p);
    ps2 = make_unique<demo>("world",2);
    cout << *ps1 << *ps2;
}