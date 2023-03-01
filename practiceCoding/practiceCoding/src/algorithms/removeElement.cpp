#include <iostream>
#include <vector>

using namespace std;

int remove(vector<int>& nums, int val) {
	int slow = 0;
	for (int fast = 0; fast < nums.size(); fast++) {
		if (val != nums[fast]) {
			nums[slow++] = nums[fast];
		}
	}
	return slow;
}

int main() {
	vector<int> a = { 0,1,2,3,4,2 };
	cout << remove(a, 2) << endl;
}