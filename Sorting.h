#pragma once

#include <iterator>
#include <type_traits>

namespace utilities
{

#pragma region Heap Sort

template < typename IteratorT, typename CompareT = std::less<typename IteratorT::value_type> >
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

template < typename IteratorT, typename CompareT = std::less<typename IteratorT::value_type> >
void HeapInsert(IteratorT first, IteratorT inserted, CompareT const & compare = CompareT())
{
   HeapInsert(first, inserted, std::distance (first, inserted), compare);
}

template < typename IteratorT, typename CompareT = std::less<typename IteratorT::value_type> >
void HeapMake (IteratorT first, IteratorT last, CompareT const & compare = CompareT ())
{
   for (auto current = std::make_pair (first, 0); current.first != last; ++current.first, ++current.second)
   {
      HeapInsert (first, current.first, current.second, compare);
   }
}

template < typename IteratorT, typename CompareT = std::less<typename IteratorT::value_type> >
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


template < typename IteratorT, typename CompareT = std::less<typename IteratorT::value_type> >
IteratorT HeapRemoveTop (IteratorT first, IteratorT last, CompareT const & compare = CompareT ())
{
   last = std::prev (last);
   if (first != last)
   {
      std::swap(*first, *last);

      HeapInsertTop(first, 0, std::distance(first, last), compare);
   }
   return last;
}

template < typename IteratorT, typename CompareT = std::less<typename IteratorT::value_type> >
void HeapSort (IteratorT first, IteratorT last, CompareT const & compare = CompareT())
{
   while (first != last)
   {
      last = HeapRemoveTop (first, last, compare);
   }
}

#pragma endregion

#pragma region Merge Sort



#pragma endregion
}