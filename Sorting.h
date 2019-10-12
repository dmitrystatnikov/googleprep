#pragma once

#include <algorithm>
#include <iterator>
#include <type_traits>
#include <vector>

namespace utilities
{

#pragma region Heap Sort

template <typename IteratorT> using DefaultCompareT = std::less<typename std::iterator_traits<IteratorT>::value_type>;

template < typename IteratorT, typename CompareT = DefaultCompareT<IteratorT> >
void HeapInsert (IteratorT first, IteratorT inserted, typename std::iterator_traits<IteratorT>::difference_type distanceToInserted, CompareT const & compare = CompareT ())
{
   if (inserted == first) return;

   auto parent = first;
   auto distanceToParent = (distanceToInserted - 1) / 2;
   std::advance (parent, distanceToParent);

   if (compare (*inserted, *parent)) return;

   std::swap (*parent, *inserted);

   HeapInsert (first, parent, distanceToParent, compare);
}

template < typename IteratorT, typename CompareT = DefaultCompareT<IteratorT> >
void HeapInsert(IteratorT first, IteratorT inserted, CompareT const & compare = CompareT())
{
   HeapInsert(first, inserted, std::distance (first, inserted), compare);
}

template < typename IteratorT, typename CompareT = DefaultCompareT<IteratorT> >
void HeapMake (IteratorT first, IteratorT last, CompareT const & compare = CompareT ())
{
   for (auto current = std::make_pair (first, 0); current.first != last; ++current.first, ++current.second)
   {
      HeapInsert (first, current.first, current.second, compare);
   }
}

template < typename IteratorT, typename CompareT = DefaultCompareT<IteratorT> >
void HeapInsertTop (IteratorT first, size_t distToParent, size_t distToEnd, CompareT const & compare = CompareT ())
{
   if (distToParent >= distToEnd) return;

   auto distToLeftChild = 2 * distToParent + 1;

   if (distToLeftChild >= distToEnd) return;

   auto parent = std::next (first, distToParent);
   auto nextParent = std::next(first, distToLeftChild);
   auto distToNextParent = distToLeftChild;

   auto distToRightChild = 2 * distToParent + 2;
   if (distToRightChild < distToEnd)
   {
      auto rightChild = std::next (first, distToRightChild);
      if (compare(*nextParent, *rightChild))
      {
         nextParent = rightChild;
         distToNextParent = distToRightChild;
      }
   }

   if (compare (*parent, *nextParent))
   {
      std::swap (*parent, *nextParent);
      HeapInsertTop (first, distToNextParent, distToEnd, compare);
   }
}


template < typename IteratorT, typename CompareT = DefaultCompareT<IteratorT> >
IteratorT HeapRemoveTop (IteratorT first, IteratorT last, CompareT const & compare = CompareT ())
{
   last = std::prev (last);
   if (first != last)
   {
      std::swap (*first, *last);

      HeapInsertTop(first, 0, std::distance (first, last), compare);
   }
   return last;
}

template < typename IteratorT, typename CompareT = DefaultCompareT<IteratorT> >
void HeapSort (IteratorT first, IteratorT last, CompareT const & compare = CompareT())
{
   while (first != last)
   {
      last = HeapRemoveTop (first, last, compare);
   }
}

#pragma endregion

#pragma region Merge Sort

template < typename IteratorT, typename CompareT = DefaultCompareT<IteratorT> >
void Merge (IteratorT first1, IteratorT const last1,
            IteratorT first2, IteratorT const last2,
            IteratorT firstOutput,
            CompareT const & compare = CompareT ())
{
   if (first1 == last1)
   {
      std::copy (first2, last2, firstOutput);
      return;
   }

   if (first2 == last2)
   {
      std::copy (first1, last1, firstOutput);
      return;
   }

   IteratorT & current = compare (*first1, *first2) ? first1 : first2;

   *firstOutput++ = *current++;

   Merge (first1, last1, first2, last2, firstOutput, compare);
}

template < typename IteratorT, typename CompareT = DefaultCompareT<IteratorT> >
void MergeSort (IteratorT first, IteratorT const last, IteratorT firstBuffer, CompareT const & compare = CompareT ())
{
   if (first == last || last == std::next (first)) return;

   auto size   = std::distance (first, last);
   auto middle = std::next (first, size / 2);

   auto middleBuffer = std::next (firstBuffer, size / 2);
   auto lastBuffer   = std::next (firstBuffer, size);

   MergeSort (first, middle, firstBuffer, compare);
   MergeSort (middle, last, middleBuffer, compare);

   Merge (first, middle, middle, last, firstBuffer, compare);

   std::copy (firstBuffer, lastBuffer, first);
}

template < typename IteratorT, typename CompareT = DefaultCompareT<IteratorT> >
void MergeSort (IteratorT first, IteratorT const last, CompareT const & compare = CompareT ())
{
   auto size = std::distance (first, last);

   std::vector <typename std::iterator_traits<IteratorT>::value_type> tmpContainer (size + size % 2);

   auto middle = std::next (first, size / 2);
   auto tmpMiddle = std::next (tmpContainer.begin (), tmpContainer.size () / 2);
   auto tmpLast = std::next (tmpContainer.begin(), size);

   std::copy (middle, last, tmpContainer.begin ());

   MergeSort (first, middle, middle, compare);
   MergeSort (tmpContainer.begin(), tmpMiddle, tmpMiddle, compare);

   std::copy (tmpContainer.begin(), tmpMiddle, middle);

   Merge (first, middle, middle, last, tmpContainer.begin (), compare);

   std::copy (tmpContainer.begin (), tmpLast, first);
}


#pragma endregion
}