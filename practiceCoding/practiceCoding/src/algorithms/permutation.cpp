
 //N�������ַ���ȫ����
#include<iostream>
#include<string>
using namespace std;
//	��str��ȫ�����㷨
//  ����str��Χ���� [a,b] 
void perm(string str, int a, int b) {
	if (a == b) {
		//		�������Ϊ b+1 
		for (int i = 0; i <= b; i++) {
			cout << str[i];
		}
		cout << endl;
	}

	for (int k = a; k <= b; k++) {
		swap(str[k], str[a]);
		perm(str, a + 1, b);
		//�ָ��ֳ�
		swap(str[k],str[a]);
	}
}
int main() {
	string str="";
	int n = str.size();
	perm(str, 0, n - 1);
}

