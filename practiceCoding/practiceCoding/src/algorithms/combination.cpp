/*λ��������ϣ���һ�����������֣��������ַ���ȡ�ᣬĳһλΪ1����ȡ��Ӧ���ַ�����Ϊ0��ȡ�����ܹ�ʵ���ַ���ϡ�
N���ַ�����Ϲ���2n-1����Ϸ�ʽ������һ������i��1������2n-1��i�Ķ����Ʊ�ʾÿ��byteλ��Ϊ1��������ַ����ж�Ӧ���ַ���
�����ַ���Ϊ��123��,��iΪ1��ʱ�������Ϊ001�����λΪ1�����Ӧ�������3��iΪ7��ʱ�������λ111�����Ӧ�����Ϊ123.*/
//Ҫ��������ַ�û���ظ������������Ͻ�������ظ���
#include<iostream>
using namespace std;
void combination(string str)
{
	int length = str.size();
	int n = 1 << length;//1������lengthλ����Ϊ100000����length+1λ.
	for (int i = 1; i < n; i++)//��length����3�����1��7���������п���ȡ���Ľ��
	{
		//int j = 1 << (length - 1);
		//int index = 0;
		//while (j >= 1)
		//{
		//	if (i & j) //λ�����㡣����011��010λ��Ľ��Ϊ010��Ϊ2������0����Ϊ��
		//		cout << str[index];
		//	index++;
		//	j = j >> 1;
		//}
		//cout << endl;
		for (int index = 0; index < length; index++) {
			int j = 1 << index;
			if (i & j) //λ�����㡣����011��010λ��Ľ��Ϊ010��Ϊ2������0����Ϊ��
				cout << str[index];
		}
		cout << endl;
	}
}
int main()
{
	string str="abc";
	combination(str);
}
