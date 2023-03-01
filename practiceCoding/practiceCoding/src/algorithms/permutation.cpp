
 //N个有序字符的全排列
#include<iostream>
#include<string>
using namespace std;
//	求str的全排列算法
//  其中str范围区间 [a,b] 
void perm(string str, int a, int b) {
	if (a == b) {
		//		输出长度为 b+1 
		for (int i = 0; i <= b; i++) {
			cout << str[i];
		}
		cout << endl;
	}

	for (int k = a; k <= b; k++) {
		swap(str[k], str[a]);
		perm(str, a + 1, b);
		//恢复现场
		swap(str[k],str[a]);
	}
}
int main() {
	string str="";
	int n = str.size();
	perm(str, 0, n - 1);
}

