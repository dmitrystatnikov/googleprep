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

        explicit Node (key_type payloadArg) : i_leftChild (nullptr), i_rightChild (nullptr), i_parent (nullptr), i_payload (payloadArg) {}

        Node const *    leftChild   () const noexcept               {return i_leftChild;}
        void            leftChild   (Node * leftChildArg) noexcept  {i_leftChild = leftChildArg;}
        Node const *    rightChild  () const noexcept               {return i_rightChild;}
        void            rightChild  (Node * rightChildArg) noexcept {i_rightChild = rightChildArg;}
        Node const *    parent      () const noexcept               {return i_parent;}
        void            parent      (Node * parentArg) noexcept     {i_parent = parentArg;}

        void            replaceChild (Node * replaced, Node * replacement) noexcept
        {
            if (replaced != i_leftChild && replaced != i_rightChild) return;

            auto & child = replaced == i_leftChild ? i_leftChild : i_rightChild;

            child = replacement;

            return;
        }

        void swap (Node & other)
        {
            std::swap (i_payload, other.i_payload);
        }

        operator key_type () const noexcept {return i_payload;}
    private:
        friend BinarySearchTree<Node>;

        Node *    leftChild   () noexcept {return const_cast<Node *> (const_cast<Node const *> (this)->leftChild ());}
        Node *    rightChild  () noexcept {return const_cast<Node *> (const_cast<Node const *> (this)->rightChild ());}
        Node *    parent      () noexcept {return const_cast<Node *> (const_cast<Node const *> (this)->parent ());}

        Node *      i_leftChild;
        Node *      i_rightChild;
        Node *      i_parent;
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

    for (auto N : tree) cout << N << " ";
    cout << endl; 

    auto it = tree.find (10);
    cout << *it << endl;

    it = tree.erase (it);
    cout << *it << endl;
    for (auto N : tree) cout << N << " ";
    cout << endl;

    it = tree.find (3);
    cout << *it << endl;

    it = tree.erase (it);
    cout << *it << endl;
    for (auto N : tree) cout << N << " ";
    cout << endl;

    it = tree.find (3);
    cout << boolalpha << (it == tree.end ()) << endl;

    it = tree.erase (it);
    cout << boolalpha << (it == tree.end ()) << endl;
    for (auto N : tree) cout << N << " ";
    cout << endl;

    it = tree.find (8);
    cout << *it << endl;

    it = tree.erase (it);
    cout << *it << endl;
    for (auto N : tree) cout << N << " ";
    cout << endl;

    it = tree.find (14);
    cout << *it << endl;

    it = tree.erase (it);
    cout << (it == tree.end ()) << endl;
    for (auto N : tree) cout << N << " ";
    cout << endl;

    it = tree.find (4);
    cout << *it << endl;

    it = tree.erase (it);
    cout << *it << endl;
    for (auto N : tree) cout << N << " ";
    cout << endl;

    tree.clear ();
    cout << (tree.begin () == tree.end ()) << endl;
    for (auto N : tree) cout << N << " ";
    cout << endl;
    cout << tree.empty () << endl;
}

int main (int argc, char** argv)
{
    SortTest ([] (auto first, auto second) { HeapMake (first, second); HeapSort (first, second); });
    SortTest ([] (auto first, auto second) { MergeSort (first, second); });
    SortTest ([] (auto first, auto second) { QuickSort (first, second); });

    TreeTest ();

    return 0;
}
