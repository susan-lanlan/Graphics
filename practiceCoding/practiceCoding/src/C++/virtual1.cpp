#include<iostream>

using namespace std;

//�������������ڹ��캯��������麯��������������
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
//����������������������Ϊ�麯��������������
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
	//���캯�������麯��
	/*A* p = new B();
	p->print();*/

	//�����������麯��
	/*Foo* pCh = new Child;
	pCh->vFunc();*/

	//�鿴�༰��ʵ��������Ĵ�С
	cout << sizeof(A) << endl;
	A a;
	cout << sizeof(a) << endl;
	return 0;
}
