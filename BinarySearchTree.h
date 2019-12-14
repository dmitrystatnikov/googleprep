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

   template <typename... ArgsT>
   std::pair<NodeT *, bool> emplace (ArgsT && ... args)
   {
      auto node    = std::make_unique<NodeT> (std::forward<ArgsT...> (args)...);

      auto [parent, child]  = findParentAndNode (*node, i_root);

      if (child)
      {
         return std::make_pair (child, false);
      }

      if (!parent)
      {
         i_root = node.release   ();
         return std::make_pair        (i_root, true);
      }

      if (compare (*node, *parent))
      {
         parent->leftChild (node.release ());
         return std::make_pair   (parent->leftChild (), true);
      }
      
      parent->rightChild   (node.release ());
      return std::make_pair      (parent->rightChild (), true);
   }

   NodeT * erase (key_type const & key);

   NodeT const * find (key_type const & key) const;

   bool empty () const noexcept;

   void clear ();

private:

   explicit BinarySearchTree (NodeT * root) noexcept
   : i_root (root)
   {
   }

   static const CompareT compare;

   std::tuple<NodeT const *, NodeT const *>   findParentAndNode (key_type const & key, NodeT const * root) const;
   std::tuple<NodeT *, NodeT *>               findParentAndNode (key_type const & key, NodeT const * root)
   {
      auto [parent, child] = static_cast<const BinarySearchTree<NodeT, CompareT> *>(this)->findParentAndNode (key, root);
      return std::make_tuple (const_cast<NodeT *> (parent), const_cast<NodeT*> (child));
   }

   NodeT const * removeRoot (NodeT const * removedRoot);

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
NodeT * BinarySearchTree<NodeT, CompareT>::erase (key_type const & key)
{
   auto [parent, removed] = findParentAndNode (key, i_root);

   if (!removed) return nullptr;

   auto replacement = removeRoot (removed);

   if (!parent)
   {
      i_root = replacement;
   }

   if (parent->leftChild () == removed)
   {
      parent->leftChild (replacement);
   }
   else
   {
      parent->rightChild (replacement);
   }
   
   delete removed;
   return replacement;
}

template < typename NodeT, typename CompareT >
NodeT const * BinarySearchTree<NodeT, CompareT>::find (key_type const & key) const
{
   auto [parent, child] = findParentAndNode (key, i_root);

   return child;
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
std::tuple<NodeT const *, NodeT const *> BinarySearchTree<NodeT, CompareT>::findParentAndNode (key_type const & key, NodeT const * root) const
{
   NodeT const * parent = nullptr;
   NodeT const * child = root;

   while (child)
   {
      if (compare (key, *child))
      {
         parent = child;
         child = parent->leftChild ();
         continue;
      }

      if (compare (*child, key))
      {
         parent = child;
         child = parent->rightChild ();
         continue;
      }

      break;
   }

   return std::make_tuple (parent, child);
}

template < typename NodeT, typename CompareT >
NodeT const * BinarySearchTree<NodeT, CompareT>::removeRoot (NodeT const * replacedRoot)
{
   assert (replacedRoot && "removeRoot: Expecting a valid node pointer.");

   auto * replacement = replacedRoot->rightChild ();

   if (!replacement) return replacedRoot->leftChild ();

   if (!replacement->leftChild ())
   {
      replacement->leftChild (replacedRoot->leftChild ());
      return replacement;
   }

   auto * replacementParent = replacement;
   replacement = replacement->leftChild ();
   while (replacement->leftChild ())
   {
      replacementParent = replacement;
      replacement = replacement->leftChild ();
   }
   replacementParent->leftChild (replacement->rightChild ());
   replacement->leftChild (replacedRoot->leftChild ());
   replacement->rightChild (replacedRoot->rightChild ());

   return replacement;
}

}
