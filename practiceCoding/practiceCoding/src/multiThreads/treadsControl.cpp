#include <thread>
#include <iostream>
using namespace std;
struct func
{
    int& i;
    func(int& i_) :i(i_) {}
    void operator() ()
    {
        /*for (int j = 0; j < 10; ++j)
        {
            cout << i << endl;
        }*/
        cout <<"from thread:"<< & i << endl;
        i = 20;
    }
};
void oops()
{
    int some_local_state = 10;
    cout << "from oops:" << &some_local_state << endl;
    func my_func(some_local_state);
    std::thread my_thread(my_func);
    //for (int i = 0; i < 10000000; i++) {}
    cout << "from oops:" << "join执行之前..." << endl;
    my_thread.join();
    cout << "from oops:" << "join执行完拉！" << endl;
    cout << "from oops: i " << some_local_state << endl;
}
class thread_guard
{
    std::thread& t;
public:
    explicit thread_guard(std::thread& t_) : t(t_)
    {
    }
    ~thread_guard()
    {
        if (t.joinable())
            t.join();
    }
    thread_guard(thread_guard const&) = delete;
    thread_guard& operator=(thread_guard const&) = delete;
};
//void f()
//{
//    int some_local_state = 10;
//    cout << "from oops:" << &some_local_state << endl;
//    func my_func(some_local_state);
//    std::thread t(my_func);
//    thread_guard g(t);
//    //for (int i = 0; i < 10000000; i++) {}
//    cout << "from f:" << "f执行完拉！" << endl;
//}
void f_2(int i, char* s) {
    cout << "from f: s的地址 " << &(s) << endl;
    cout << "from f: s指向的地址 " << static_cast<void*>(s) << endl;
    cout<<"from f: s " << s << endl;
};
void f_3(int i, string& s) {
    cout << "from f: s的地址 " << &(s) << endl;
    cout << "from f: s " << s << endl;
};
void f_1(int i, string const& s) {
    cout << "from f: s的地址 " << &(s) << endl;
    cout << "from f: s " << s << endl;
};
void oops_2(int some_para) {
    char buffer[] = "hello thread!";
    /*cout << "from oops_2: s指向的地址 " << static_cast<void*>(buffer) << endl;
    cout << "from oops_2: s " << buffer << endl;*/
    
    //std::thread t(f_2, 3, buffer);
    string temp = std::string(buffer);
    //std::thread t(f_3, 3, buffer);
    cout << "from oops_2: temp指向的地址 " << &(temp) << endl;
    cout << "from oops_2: temp " << temp << endl;
    std::thread t(f_1, 3, std::ref(temp));
    std::thread t2(f_3, 3, std::ref(temp));
    //t.detach();
    t.join();
    t2.join();
}
class X
{
public:
    void do_lengthy_work(int param) {
        cout << "from thread: " << param << endl;
    };
};
void oops_3() {
    X my_x;
    int p = 10;
    std::thread t(&X::do_lengthy_work, &my_x, p);
    t.join();
}

int main() {
    /*oops();
    cout << "from main:" << "oops已退出！" << endl;*/
    /*f();
    cout << "from main:" << "f已退出！" << endl;*/
    //oops_2(10);
    //oops_3();
    cout << std::thread::hardware_concurrency() << endl;
}