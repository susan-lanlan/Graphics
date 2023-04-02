#include<iostream>
#include<memory>

using namespace std;

class B;
class A
{
public:
	weak_ptr<B> pb_;
	//shared_ptr<B> pb_;
	~A(){
		if (!pb_.expired())
		//if (pb_ != nullptr)
			cout << "pb_ use_count:" << pb_.use_count() << endl;
		cout << "A delete" << endl;
	}
};
class B
{
public:
	shared_ptr<A> pa_;
	~B(){
		if (pa_ != nullptr) {
			cout << "pa_->pb use_count:" << pa_->pb_.use_count() << endl;
			cout << "pa_ use_count:" << pa_.use_count() << endl;
		}
			
		cout << "B delete" << endl;
	}
};
void fun()
{
	//创建两个shared_ptr
	shared_ptr<B> pb(new B());
	shared_ptr<A> pa(new A());
	//给对象成员赋值
	pb->pa_ = pa;
	pa->pb_ = pb;
	cout << "pa use_count:" << pa.use_count() << endl;
	cout << "pb use_count:" << pb.use_count() << endl;
}
int main()
{
	fun();
	cout << "shared ptr" << endl;
	return 0;
}
