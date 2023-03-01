#include <iostream>
#include <vector>

using namespace std;

class Node {
public:
    Node* left;
    Node* right;
    int val;

    Node();
    Node(int v) :left(nullptr), right(nullptr), val(v) {};

};
void insertNode(Node* root, int val) {
    if (val < root->val) {
        if (root->left == nullptr)
            root->left = new Node(val);
        else
            insertNode(root->left, val);
    }
    else {
        if (root->right == nullptr)
            root->right = new Node(val);
        else
            insertNode(root->right, val);
    }
}
Node* buildBST (const vector<int> &arr)
{
    if (arr.size() < 1)
        return nullptr;
    Node* root = new Node(arr[0]);
    if (arr.size() < 2)
        return root;
    for (int i = 1; i < arr.size(); i++) {
        insertNode(root, arr[i]);
    }
    return root;

}
void InorderTraversal(Node* root) {
    if (root == nullptr)
        return;
    if (root->left != nullptr)
        InorderTraversal(root->left);
    cout << root->val;
    if(root->right != nullptr)
        InorderTraversal(root->right);
}
int main() {
    vector<int> a{ 1,5,3,7,6,8,2,4,9 };
    Node* root = buildBST(a);
    InorderTraversal(root);
    return 0;
}
