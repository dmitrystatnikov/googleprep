#pragma once

#include <cassert>
#include <functional>
#include <memory>
#include <stack>
#include <tuple>
#include <utility>
#include <vector>

namespace utilities
{

template < typename NodeT, typename CompareT = std::less<typename NodeT::key_type> >
class BinarySearchTree
{
   using Self_t = BinarySearchTree<NodeT, CompareT>;
public:

   using key_type = typename NodeT::key_type;

   BinarySearchTree  ();

   ~BinarySearchTree ();

   class iterator : public std::iterator <std::bidirectional_iterator_tag, NodeT>
   {
      using base_t = std::iterator <std::bidirectional_iterator_tag, NodeT>;
   public:

      using reference = typename base_t::reference;
      using pointer = typename base_t::pointer;

      iterator () : i_current (nullptr) {};

      iterator (iterator const &)               = default;
      iterator (iterator &&)                    = default;
      iterator & operator = (iterator const &)  = default;
      iterator & operator = (iterator &&)       = default;

      reference         operator * () {return *i_current;}

      pointer           operator ->() {return i_current;}

      iterator & operator ++ ()     {auto nextPtr = rightSubTreeMinimum (); i_current = nextPtr ? nextPtr : leftSubTreeParent (); return *this;}
      iterator   operator ++ (int)  {auto res = *this; ++(*this); return res;}
      iterator & operator -- ()     {auto prevPtr = leftSubTreeMaximum (); i_current = prevPtr ? prevPtr : rightSubTreeParent (); return *this;}
      iterator   operator -- (int)  {auto res = *this; --(*this); return res;}

      bool operator == (iterator const & it) const noexcept {return i_current == it.i_current;}
      bool operator != (iterator const & it) const noexcept {return i_current != it.i_current;}

   private:
      friend BinarySearchTree<NodeT, CompareT>;

      iterator (NodeT * current) : i_current (current) {}

      NodeT * get () {return i_current;}

      NodeT * rightSubTreeMinimum ()
      {
         if (!i_current) return nullptr;
         auto nextPtr = i_current->rightChild ();
         while (nextPtr && nextPtr->leftChild ()) nextPtr = nextPtr->leftChild ();
         return nextPtr;
      }

      NodeT * leftSubTreeParent   ()
      {
         if (!i_current) return nullptr;
         auto nextPtr = i_current;
         while (nextPtr->parent () && nextPtr == nextPtr->parent ()->rightChild ()) nextPtr = nextPtr->parent ();
         return nextPtr->parent ();
      }

      NodeT * leftSubTreeMaximum  ()
      {
         if (!i_current) return nullptr;
         auto nextPtr = i_current->leftChild ();
         while (nextPtr && nextPtr->rightChild ()) nextPtr = nextPtr->rightChild ();
         return nextPtr;
      }

      NodeT * rightSubTreeParent  ()
      {
         if (!i_current) return nullptr;
         auto nextPtr = i_current;
         while (nextPtr->parent () && nextPtr == nextPtr->parent ()->leftChild ()) nextPtr = nextPtr->parent ();
         return nextPtr->parent ();
      }

      NodeT * i_current;
   };

   template <typename... ArgsT>
   std::pair<iterator, bool> emplace (ArgsT && ... args)
   {
      auto node    = std::make_unique<NodeT> (std::forward<ArgsT...> (args)...);

      auto child = find (*node);
      if (child != end ()) return make_pair (child, false);

      auto parent  = findParent (*node);

      auto current = node.release ();

      if (parent == end ())
      {
         i_root = current;
      }
      else if (compare (*current, *parent))
      {
         parent->leftChild (current);
      }
      else
      {
         assert (compare (*parent, *current) && "Expecting case when parent smaller then inserted node.");

         parent->rightChild (current);
      }

      current->parent (parent.get ());
      return std::make_pair (iterator (current), true);
   }

   iterator erase (iterator it);

   iterator find  (key_type const & key) const;

   bool empty () const noexcept;

   void clear ();

   iterator begin () {auto current = i_root; while (current && current->leftChild ()) current = current->leftChild (); return iterator (current);}
   iterator end   () {return iterator ();}

private:

   explicit BinarySearchTree (NodeT * root) noexcept
   : i_root (root)
   {
   }

   static const CompareT compare;

   iterator findParent (key_type const & key) const;
   iterator findParent (key_type const & key)
   {
      return static_cast<const BinarySearchTree<NodeT, CompareT> *>(this)->findParent (key);
   }

   NodeT * i_root;
};

template < typename NodeT, typename CompareT >
CompareT const BinarySearchTree<NodeT, CompareT>::compare = CompareT ();

template < typename NodeT, typename CompareT >
BinarySearchTree<NodeT, CompareT>::BinarySearchTree ()
: i_root {nullptr}
{
}

template < typename NodeT, typename CompareT >
BinarySearchTree<NodeT, CompareT>::~BinarySearchTree ()
{
   clear ();
}

template < typename NodeT, typename CompareT >
typename BinarySearchTree<NodeT, CompareT>::iterator BinarySearchTree<NodeT, CompareT>::erase (iterator it)
{
   auto result = end ();

   if (it == end ()) return result;

   auto replacement = it.leftSubTreeMaximum ();
   if (!replacement)
   {
      replacement = it.rightSubTreeMinimum ();
      result = it;
   }

   if (!replacement)
   {
      ++result;
      if (it->parent ()) it->parent ()->replaceChild (it.get (), nullptr);
      it->parent (nullptr);

      delete it.get ();
      return result;
   }

   it->swap (*replacement);

   auto child = replacement->leftChild () ? replacement->leftChild () : replacement->rightChild ();
   if (child) child->parent (replacement->parent ());

   replacement->parent ()->replaceChild (replacement, child);
   replacement->replaceChild (child, nullptr);
   replacement->parent (nullptr);

   delete replacement;

   return result == end () ? ++it : result;
}

template < typename NodeT, typename CompareT >
typename BinarySearchTree<NodeT, CompareT>::iterator BinarySearchTree<NodeT, CompareT>::find (key_type const & key) const
{
   auto current = i_root; 
   while (current && (compare (*current, key) || compare (key, *current)))
   {
      current = compare (*current, key) ? current->rightChild () : current->leftChild ();      
   }

   return iterator (current);
}

template < typename NodeT, typename CompareT >
bool BinarySearchTree<NodeT, CompareT>::empty () const noexcept
{
   return i_root == nullptr;
}

template < typename NodeT, typename CompareT >
void BinarySearchTree<NodeT, CompareT>::clear ()
{
   if (!i_root) return;

   if (i_root->leftChild ()) i_root->leftChild ()->parent (nullptr);
   if (i_root->rightChild ()) i_root->rightChild ()->parent (nullptr);

   Self_t leftTree (i_root->leftChild ());
   Self_t rightTree (i_root->rightChild ());

   i_root->leftChild (nullptr);
   i_root->rightChild (nullptr);

   delete i_root;
   i_root = nullptr;

   leftTree.clear ();
   rightTree.clear ();
}

template < typename NodeT, typename CompareT >
typename BinarySearchTree<NodeT, CompareT>::iterator BinarySearchTree<NodeT, CompareT>::findParent (key_type const & key) const
{
   auto current = i_root; 
   while (current)
   {
      if (compare (*current, key) && current->rightChild ())
      {
         current = current->rightChild ();
         continue;
      } 
           
      if (compare (key, *current) && current->leftChild ())
      {
         current = current->leftChild ();
         continue;
      }  

      break;    
   }

   if (current && current == i_root && !(compare (*current, key) || compare (key, *current))) current = nullptr;

   return iterator (current);
}

}
