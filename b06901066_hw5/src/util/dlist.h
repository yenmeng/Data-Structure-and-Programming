/****************************************************************************
  FileName     [ dlist.h ]
  PackageName  [ util ]
  Synopsis     [ Define doubly linked list package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef DLIST_H
#define DLIST_H

#include <cassert>
#include <algorithm>

template <class T> class DList;

// DListNode is supposed to be a private class. User don't need to see it.
// Only DList and DList::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class DListNode
{
   friend class DList<T>;
   friend class DList<T>::iterator;

   DListNode(const T& d, DListNode<T>* p = 0, DListNode<T>* n = 0):
      _data(d), _prev(p), _next(n) {}

   // [NOTE] DO NOT ADD or REMOVE any data member
   T              _data;
   DListNode<T>*  _prev;
   DListNode<T>*  _next;
};


template <class T>
class DList
{
public:
   // TODO: decide the initial value for _isSorted
   DList() {
      _head = new DListNode<T>(T());
      _head->_prev = _head->_next = _head; // _head is a dummy node
   }
   ~DList() { clear(); delete _head; }

   // DO NOT add any more data member or function for class iterator
   class iterator
   {
      friend class DList;

   public:
      iterator(DListNode<T>* n= 0): _node(n) {}
      iterator(const iterator& i) : _node(i._node) {}
      ~iterator() {} // Should NOT delete _node

      // TODO: implement these overloaded operators
      const T& operator * () const { return *(this); }
      T& operator * () { return _node->_data; }
      iterator& operator ++ () { _node=_node->_next; return *(this); }
      iterator operator ++ (int) { iterator temp = *this; _node=_node->_next; return temp; }
      iterator& operator -- () { _node=_node->_prev; return *(this); }
      iterator operator -- (int) { iterator temp = *this; _node=_node->_prev; return temp; }

      iterator& operator = (const iterator& i) { _node=i._node; return *(this); }

      bool operator != (const iterator& i) const { return _node!=i._node; }
      bool operator == (const iterator& i) const { return _node==i._node; }

   private:
      DListNode<T>* _node;
   };

   // TODO: implement these functions
   iterator begin() const { return iterator(_head); }
   iterator end() const { return iterator(_head->_prev); }
   bool empty() const { return (_head==_head->_next); }
   size_t size() const {
       size_t n=0;
       DListNode<T>* t = _head;
       while(t->_next!=_head){
           n++;
           t=t->_next;
       }
       return n;
   }

   void push_back(const T& x) {
       if(empty()){
           DListNode<T>* p = new DListNode<T>(x);
           p->_prev=_head->_prev;
           p->_next=_head;
           _head=p;
           _head->_next->_prev=_head;
           _head->_prev->_next=_head;
       }
       else {
           DListNode<T>* p = new DListNode<T>(x,_head->_prev->_prev,_head->_prev);
           (_head->_prev->_prev)->_next=p;
           _head->_prev->_prev=p;
       }
       _isSorted=false;
   }
   void pop_front() {
       if (empty()) return;
       if(size()>=2){
           DListNode<T>* t = _head->_next;
           _head->_prev->_next=t;
           _head->_next->_prev=_head->_prev;
           delete _head;
           _head=t;
       }
       else {
           _head->_prev = _head->_next = _head;//only one element(reset head to dummy)
       }
   }
   void pop_back() {
       if(empty()) return;
       if(size()>=2){
           DListNode<T>* last_ele = _head->_prev->_prev;
           last_ele->_prev->_next = last_ele->_next;
           last_ele->_next->_prev = last_ele->_prev;
           delete last_ele;
           last_ele=_head->_prev->_prev;
       }
       else {
           _head->_prev = _head->_next = _head;//reset
       }
   }

   // return false if nothing to erase
   bool erase(iterator pos) {
       if(empty()) return false;
       if(pos==begin()) pop_front();
       else{
           DListNode<T>* t = pos._node;
           pos._node->_prev->_next=pos._node->_next;
           pos._node->_next->_prev=pos._node->_prev;
           delete t;
           t=0;
       }
       return true;
   }
   bool erase(const T& x) {
       if(find(x)==end()) return false;
       else {
           erase(find(x));
           return true;
       }
   }

   iterator find(const T& x) {
       for(DList<T>::iterator li= begin();li!=end();li++){
           if(li._node->_data==x) return li;
       }
       return end();
   }

   void clear() {
       if(empty()) return ;
       while(!empty()){pop_front();}
       _isSorted=true;
   }  // delete all nodes except for the dummy node

   void sort() const{
       if(empty()) return;
       if(_isSorted) return;
       if(size()==1) {_isSorted=true; return;}
       if(!_isSorted){
           DListNode<T> *ii=_head;
           size_t j=0;
           size_t i=0;
           T temp=ii->_data;
           while(ii->_next!=_head){
               temp=ii->_data;
               DListNode<T>* jj=ii;
               j=i-1;
               if(j>=0) jj=ii->_prev;
               while (jj->_next!= _head &&  jj->_data > temp) {
                   jj->_next->_data=jj->_data;
                   jj=jj->_prev;
               }
               jj->_next->_data=temp;
               ii=ii->_next;
               ++i;
           }
       }
       _isSorted=true;
   }

private:
   // [NOTE] DO NOT ADD or REMOVE any data member
   DListNode<T>*  _head;     // = dummy node if list is empty
   mutable bool   _isSorted; // (optionally) to indicate the array is sorted

   // [OPTIONAL TODO] helper functions; called by public member functions
};

#endif // DLIST_H
