#include<iostream>

using namespace std;

//——————在构造函数里调用虚函数——————
class A {
public:
	int x;
	A() {
		cout << "construct A" << endl;
		print();
	}
	virtual void print() {
		cout << "A" << endl;
	}
};
class B: public A {
public:
	B() {
		cout << "construct B" << endl;
	}
	virtual void print() {
		cout << "B" << endl;
	}
};
//——————内联函数作为虚函数——————
class Foo
{
public:
	virtual void vFunc() const;
};


class Child : public Foo
{
public:
	virtual void vFunc() const;
};


inline void Foo::vFunc() const
{
	cout << "class Foo" << endl;
}

inline void Child::vFunc() const
{
	cout << "class Child" << endl;
}



int main()
{
	//构造函数调用虚函数
	/*A* p = new B();
	p->print();*/

	//内联函数做虚函数
	/*Foo* pCh = new Child;
	pCh->vFunc();*/

	//查看类及其实例化对象的大小
	cout << sizeof(A) << endl;
	A a;
	cout << sizeof(a) << endl;
	return 0;
}
