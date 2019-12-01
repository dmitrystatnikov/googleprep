#include <algorithm>
#include <iostream>
#include <vector>

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

int main (int argc, char** argv)
{
    SortTest ([] (auto first, auto second) { HeapMake (first, second); HeapSort (first, second); });
    SortTest ([] (auto first, auto second) { MergeSort (first, second); });
    SortTest ([] (auto first, auto second) { QuickSort (first, second); });

    return 0;
}
