/*位运算求组合，用一个二进制数字，来决定字符的取舍，某一位为1，则取对应的字符，若为0则不取，就能够实现字符组合。
N个字符的组合共有2n-1种组合方式，设置一个变量i从1增长到2n-1，i的二进制表示每个byte位上为1，则输出字符串中对应的字符。
例如字符串为”123”,当i为1的时候二进制为001，最低位为1，则对应的组合是3，i为7的时候二进制位111，则对应的组合为123.*/
//要求输入的字符没有重复，否则输出组合结果中有重复的
#include<iostream>
using namespace std;
void combination(string str)
{
	int length = str.size();
	int n = 1 << length;//1向左移length位，变为100000，共length+1位.
	for (int i = 1; i < n; i++)//若length等于3，则从1到7，既是所有可能取到的结果
	{
		//int j = 1 << (length - 1);
		//int index = 0;
		//while (j >= 1)
		//{
		//	if (i & j) //位与运算。比如011与010位与的结果为010，为2，不是0所以为真
		//		cout << str[index];
		//	index++;
		//	j = j >> 1;
		//}
		//cout << endl;
		for (int index = 0; index < length; index++) {
			int j = 1 << index;
			if (i & j) //位与运算。比如011与010位与的结果为010，为2，不是0所以为真
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
