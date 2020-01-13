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

template <typename ArgT, typename ResT>
struct AssignOnConst
{
   using type = ResT;
};

template <typename ArgT, typename ResT>
struct AssignOnConst <const ArgT, ResT>
{
   using type = ResT const;
};

template < typename PayloadT, typename CompareT = std::less <PayloadT> >
class BinarySearchTree
{
   using Self_t = BinarySearchTree<PayloadT, CompareT>;
public:

   #pragma region Construction, Dectruction, Assignment

   BinarySearchTree ();

   BinarySearchTree (BinarySearchTree const &);

   BinarySearchTree (BinarySearchTree &&);

   BinarySearchTree (std::initializer_list<PayloadT> && Args);

   BinarySearchTree & operator = (BinarySearchTree const &);

   BinarySearchTree & operator = (BinarySearchTree &&);

   ~BinarySearchTree ();

   #pragma endregion

   #pragma region Iterator

private:
   class Node;

   template <typename T> class iterator_base; 

public:
   using const_iterator          = iterator_base <PayloadT const>;

   using iterator                = iterator_base <PayloadT>;

   using const_reverse_iterator  = std::reverse_iterator <const_iterator>;

   using reverse_iterator        = std::reverse_iterator <iterator>;

   const_iterator          begin    () const;
   const_iterator          cbegin   () const;
   const_iterator          end      () const;
   const_iterator          cend     () const;
   iterator                begin    ();
   iterator                end      ();

   const_reverse_iterator rbegin    () const;
   const_reverse_iterator rcbegin   () const;
   const_reverse_iterator rend      () const;
   const_reverse_iterator rcend     () const;
   reverse_iterator       rbegin    ();
   reverse_iterator       rend      ();

   #pragma endregion

   #pragma region Accessors

   const_iterator find  (PayloadT const & value) const noexcept;

   iterator       find  (PayloadT const & value) noexcept;

   bool           empty () const noexcept;

   #pragma endregion

   #pragma region Modifiers

   using insertion_t = std::pair<iterator, bool>;

   insertion_t emplace  (PayloadT && value);

   template <typename... ArgsT>
   insertion_t emplace  (ArgsT&&... args) {return emplace (PayloadT (args...));}

   iterator    erase    (iterator position);
   
   void        clear    ();

   const_iterator rotateLeft (const_iterator position);

   const_iterator rotateRight (const_iterator position);

   #pragma endregion

private:

   #pragma region Node declaration / definition

   class Node
   {
   public:
      
      Node () = delete;
      Node (PayloadT const & value) : i_parent (nullptr), i_left (nullptr), i_right (nullptr), i_value (value) {}
      Node (PayloadT && value) : i_parent (nullptr), i_left (nullptr), i_right (nullptr), i_value (std::move (value)) {}

      PayloadT const &  value () const {return i_value;}
      PayloadT &        value () {return const_cast<PayloadT &> (const_cast <Node const *> (this)->value ());}

      Node const *   parent () const {return i_parent;}
      Node *         parent () {return const_cast<Node *> (const_cast <Node const *> (this)->parent ());}

      Node const *   left () const {return i_left;}
      Node *         left () {return const_cast<Node *> (const_cast <Node const *> (this)->left ());}

      Node const *   right () const {return i_right;}
      Node *         right () {return const_cast<Node *> (const_cast <Node const *> (this)->right ());}

      void           replaceChild (Node * child, Node * replacement)
      {
         if (child != i_left && child != i_right) return;

         auto & replaced = child == i_left ? i_left : i_right;

         replaced = replacement;
      }

   private:

      friend BinarySearchTree <PayloadT, CompareT>;

      void           parent   (Node * node) {i_parent = node;}
      void           left     (Node * node) {i_left = node;}
      void           right    (Node * node) {i_right = node;}

      void swap (Node * other);

      Node * i_parent;
      Node * i_left;
      Node * i_right;

      PayloadT i_value;
   };

   #pragma endregion

   #pragma region iterator base

   template <typename T>
   class iterator_base : public std::iterator <std::bidirectional_iterator_tag, T>
   {
      using base_t = std::iterator <std::bidirectional_iterator_tag, T>;

      using self_t = iterator_base <T>;

      using node_t = typename AssignOnConst <T, Node>::type;

   public:

      using value_type        = typename base_t::value_type;

      using difference_type   = typename base_t::difference_type;

      using reference         = typename base_t::reference;

      using pointer           = typename base_t::pointer;

      iterator_base () : i_current (nullptr) {}

      iterator_base (node_t * current) : i_current (current) {}

      iterator_base (iterator_base const &)  = default;

      iterator_base (iterator_base &&)       = default;

      ~iterator_base () { i_current = nullptr; }
   
      reference   operator *  () {return i_current->value ();}

      pointer     operator->  () {return &(i_current->value ());}

      self_t & operator ++ ()    {auto next = minRightSubTree (i_current); i_current = next ? next : parentOfLeftSubTree (i_current); return *this;}

      self_t   operator ++ (int) {auto result = *this; ++(*this); return result;}

      self_t & operator -- ()    {auto prev = maxLeftSubTree (i_current); i_current = prev ? prev : parentOfRightSubTree (i_current); return *this;}

      self_t   operator -- (int) {auto result = *this; --(*this); return result;}

      bool     operator == (self_t it) const {return i_current == it.i_current;}

      bool     operator != (self_t it) const {return !(*this == it);}

      self_t & operator = (self_t const & it)   = default;

      self_t & operator = (self_t && it)        = default;

   private:

      friend BinarySearchTree <PayloadT, CompareT>;

      template <typename FirstDirectionT, typename SecondDirectionT>
      static node_t * extremeOfSubTree (node_t * current, FirstDirectionT fDirection, SecondDirectionT sDirection)
      {
         if (!current) return nullptr;

         current = fDirection (current);

         if (!current) return nullptr;

         auto next = sDirection (current);

         while (next)
         {
            current = next;
            next = sDirection (current);
         }

         return current;
      }

      static node_t * minRightSubTree (node_t * current)
      {
         return extremeOfSubTree (current, [](node_t * n) {return n->right ();}, [](node_t * n) {return n->left ();});
      }

      static node_t * maxLeftSubTree (node_t * current)
      {
         return extremeOfSubTree (current, [](node_t * n) {return n->left ();}, [](node_t * n) {return n->right ();});
      }

      template <typename FirstDirectionT>
      static node_t * parentOfSubTree (node_t * current, FirstDirectionT fDirection)
      {
         if (!current) return nullptr;

         auto parent = current->parent ();

         while (parent && fDirection (parent) == current)
         {
            current = parent;
            parent = current->parent ();
         }

         return parent;
      }

      static node_t * parentOfLeftSubTree (node_t * current)
      {
         return parentOfSubTree (current, [](node_t * n) {return n->right ();});
      }

      static node_t * parentOfRightSubTree (node_t * current)
      {
         return parentOfSubTree (current, [](node_t * n) {return n->left ();});
      }

      node_t * i_current;
   };

   #pragma endregion

   static Node *  DeepCopy (Node * root, Node * parent);

   static void    DeleteTree (Node * root);

   static const CompareT f_compare;

   Node * i_root;
};

template < typename PayloadT, typename CompareT >
const CompareT BinarySearchTree <PayloadT, CompareT>::f_compare {};

template < typename PayloadT, typename CompareT >
BinarySearchTree <PayloadT, CompareT>::BinarySearchTree ()
: i_root (nullptr)
{
}

template < typename PayloadT, typename CompareT >
BinarySearchTree <PayloadT, CompareT>::BinarySearchTree (BinarySearchTree const & tree)
: i_root (DeepCopy (tree.i_root, nullptr))
{
}

template < typename PayloadT, typename CompareT >
BinarySearchTree <PayloadT, CompareT>::BinarySearchTree (BinarySearchTree && tree)
: i_root (tree.i_root)
{
   tree.i_root = nullptr;
}

template < typename PayloadT, typename CompareT >
BinarySearchTree <PayloadT, CompareT>::BinarySearchTree (std::initializer_list<PayloadT> && Values)
: i_root (nullptr)
{
   for (auto && V : Values)
   {
      emplace (V);
   }
}

template < typename PayloadT, typename CompareT >
BinarySearchTree <PayloadT, CompareT>::~BinarySearchTree ()
{
   clear ();
}

template < typename PayloadT, typename CompareT >
typename BinarySearchTree <PayloadT, CompareT>::Node * BinarySearchTree <PayloadT, CompareT>::DeepCopy (Node * root, Node * parent)
{
   if (!root) return nullptr;

   auto current = new Node (root->value ());

   current->parent   (parent);
   current->left     (DeepCopy (root->left (), current));
   current->right    (DeepCopy (root->right (), current));
}

template < typename PayloadT, typename CompareT >
void BinarySearchTree <PayloadT, CompareT>::DeleteTree (Node * root)
{
   if (!root) return;

   if (root->parent ()) root->parent ()->replaceChild (root, nullptr);
   root->parent (nullptr);

   DeleteTree (root->left ());
   DeleteTree (root->right ());

   delete root;
}

template < typename PayloadT, typename CompareT >
typename BinarySearchTree <PayloadT, CompareT>::const_iterator BinarySearchTree <PayloadT, CompareT>::find (PayloadT const & value) const noexcept
{
   auto current = i_root;

   while (current && (f_compare (current->value (), value) || (f_compare (value, current->value ()))))
   {
      current = f_compare (current->value (), value) ? current->right () : current->left ();
   }

   return const_iterator (current);
}

template < typename PayloadT, typename CompareT >
typename BinarySearchTree <PayloadT, CompareT>::iterator BinarySearchTree <PayloadT, CompareT>::find (PayloadT const & value) noexcept
{
   auto it = const_cast<Self_t const *>(this)->find (value);

   return iterator (const_cast<Node *> (it.i_current));
}

template < typename PayloadT, typename CompareT >
bool BinarySearchTree <PayloadT, CompareT>::empty () const noexcept
{
   return i_root == nullptr;
}

template < typename PayloadT, typename CompareT >
typename BinarySearchTree<PayloadT, CompareT>::insertion_t BinarySearchTree<PayloadT, CompareT>::emplace (PayloadT && value)
{
   Node * parent = nullptr;
   Node * current = i_root;

   while (current && (f_compare (current->value (), value) || (f_compare (value, current->value ()))))
   {
      parent = current;
      current = f_compare (current->value (), value) ? current->right () : current->left ();
   }

   if (current) return make_pair (iterator (current), false);

   current = new Node (std::move (value));

   current->parent (parent);

   auto result = make_pair (iterator (current), true);

   if (!parent)
   {
      i_root = current;

      return result;
   }

   if (f_compare (parent->value (), value))
   {
      parent->right (current);
   }
   else
   {
      parent->left (current);
   }

   return result;
}

template < typename PayloadT, typename CompareT >
typename BinarySearchTree<PayloadT, CompareT>::iterator BinarySearchTree<PayloadT, CompareT>::erase (iterator it)
{
   auto current = it.i_current;

   iterator result = iterator (current);

   if (!current) return result;

   if (i_root == current) i_root = nullptr;

   ++result;

   if (current->left () && current->right ())
   {
      current->swap (result.i_current);

      if (!i_root) i_root = result.i_current;
   }

   auto child = current->left () ? current->left () : current->right ();

   if (!i_root) i_root = child;

   if (current->parent ()) current->parent ()->replaceChild (current, child);
   if (child) child->parent (current->parent ());

   current->parent (nullptr);
   current->left (nullptr);
   current->right (nullptr);

   delete current;

   return result;
}

template < typename PayloadT, typename CompareT >
void BinarySearchTree<PayloadT, CompareT>::clear ()
{
   DeleteTree (i_root);

   i_root = nullptr;
}

template < typename PayloadT, typename CompareT >
void BinarySearchTree<PayloadT, CompareT>::Node::swap (Node * other)
{
   if (!other)
   {
      other = this;
      return;
   }

   if (i_parent) i_parent->replaceChild (this, other);
   if (other->parent ()) other->parent ()->replaceChild (other, this);
   std::swap (i_parent, other->i_parent);

   if (i_left) i_left->parent (other);
   if (other->i_left) other->i_left->parent (this);
   std::swap (i_left, other->i_left);

   if (i_right) i_right->parent (other);
   if (other->i_right) other->i_right->parent (this);
   std::swap (i_right, other->i_right);
}

template < typename PayloadT, typename CompareT >
typename BinarySearchTree<PayloadT, CompareT>::const_iterator BinarySearchTree<PayloadT, CompareT>::begin () const
{
   return cbegin ();
}

template < typename PayloadT, typename CompareT >
typename BinarySearchTree<PayloadT, CompareT>::const_iterator BinarySearchTree<PayloadT, CompareT>::cbegin () const
{
   auto current = i_root;

   while (current && current->left ()) current = current->left ();

   return const_iterator (current);
}

template < typename PayloadT, typename CompareT >
typename BinarySearchTree<PayloadT, CompareT>::const_iterator BinarySearchTree<PayloadT, CompareT>::end () const
{
   return cend ();
}

template < typename PayloadT, typename CompareT >
typename BinarySearchTree<PayloadT, CompareT>::const_iterator BinarySearchTree<PayloadT, CompareT>::cend () const
{
   return const_iterator ();
}

template < typename PayloadT, typename CompareT >
typename BinarySearchTree<PayloadT, CompareT>::iterator       BinarySearchTree<PayloadT, CompareT>::begin ()
{
   auto it = const_cast <Self_t const *> (this)->begin ();
   auto current = const_cast<Node *> (it.i_current);
   return iterator (current);
}

template < typename PayloadT, typename CompareT >
typename BinarySearchTree<PayloadT, CompareT>::iterator       BinarySearchTree<PayloadT, CompareT>::end ()
{
   return iterator ();
}



}
