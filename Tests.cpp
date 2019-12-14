#include <algorithm>
#include <iostream>
#include <vector>

#include "BinarySearchTree.h"
#include "Sorting.h"

using namespace std;
using namespace utilities;

ostream_iterator<int> itOut (cout,", ");

template <typename SortMethodT>
void SortTest (SortMethodT SortMethod)
{
    vector<int> Unordered {5, 2, 3, 7, 3, 8, 6, 9};

    SortMethod (Unordered.begin (), Unordered.end ());

    copy (Unordered.begin (), std::prev (Unordered.end ()), itOut);
    std::cout << Unordered.back () << endl;
}

void TreeTest ()
{
    struct Node
    {
    public:
        using key_type = int;

        explicit Node (key_type payloadArg) : i_leftChild (nullptr), i_rightChild (nullptr), i_payload (payloadArg) {}

        Node const *    leftChild   () const noexcept               {return i_leftChild;}
        void            leftChild   (Node * leftChildArg) noexcept  {i_leftChild = leftChildArg;}
        Node const *    rightChild  () const noexcept               {return i_rightChild;}
        void            rightChild  (Node * rightChildArg) noexcept {i_rightChild = rightChildArg;}

        operator key_type () const noexcept {return i_payload;}
    private:
        friend BinarySearchTree<Node>;

        Node *    leftChild   () noexcept {return const_cast<Node *> (const_cast<Node const *> (this)->leftChild ());}
        Node *    rightChild  () noexcept {return const_cast<Node *> (const_cast<Node const *> (this)->rightChild ());}

        Node *      i_leftChild;
        Node *      i_rightChild;
        key_type    i_payload;
    };

    BinarySearchTree<Node> tree;
    tree.emplace (8);
    tree.emplace (3);
    tree.emplace (1);
    tree.emplace (6);
    tree.emplace (4);
    tree.emplace (7);
    tree.emplace (10);
    tree.emplace (14);
    tree.emplace (13);

    tree.traverseInOrder ([](int val) {cout << val << ", ";});
    cout << endl;
}

int main (int argc, char** argv)
{
    SortTest ([] (auto first, auto second) { HeapMake (first, second); HeapSort (first, second); });
    SortTest ([] (auto first, auto second) { MergeSort (first, second); });
    SortTest ([] (auto first, auto second) { QuickSort (first, second); });

    TreeTest ();

    return 0;
}
