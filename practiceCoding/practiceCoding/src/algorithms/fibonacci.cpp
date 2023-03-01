#include <iostream>
using namespace std;

int fibonacci(int first, int second, int n){
	if (n <= 0) {
		return 0;
	}
	if (n < 3) {
		return 1;
	}
	else if (n == 3) {
		return first + second;
	}
	else {
		return fibonacci(second, first + second, n - 1);
	}
}

int main(){
	cout << fibonacci(0, 1, 7) << endl;
}