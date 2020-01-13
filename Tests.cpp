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
    BinarySearchTree<int> tree {8, 3, 1, 6, 4, 7, 10, 14, 13};

    auto it = tree.begin ();

    for (auto N : tree) cout << N << " ";
    cout << endl; 

    it = tree.find (10);
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
