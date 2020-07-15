/****************************************************************************
  FileName     [ bst.h ]
  PackageName  [ util ]
  Synopsis     [ Define binary search tree package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef BST_H
#define BST_H

#include <cassert>
#include <iostream>
using namespace std;

template <class T> class BSTree;

// BSTreeNode is supposed to be a private class. User don't need to see it.
// Only BSTree and BSTree::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class BSTreeNode
{
   // TODO: design your own class!!
    friend class BSTree<T>;
    friend class BSTree<T>::iterator;
    
    BSTreeNode(const T& d,BSTreeNode<T>* l = 0, BSTreeNode<T>* r = 0, BSTreeNode<T>* p = 0):
    _data(d), _left(l), _right(r), _parent(p) {};
    
    //data member
    T _data;
    BSTreeNode<T>* _left;
    BSTreeNode<T>* _right;
    BSTreeNode<T>* _parent;
   
};


template <class T>
class BSTree
{
   // TODO: design your own class!!
    public:
    BSTree():_root(0) {};
    ~BSTree(){ clear(); }
    
    class iterator
    {
        friend class BSTree<T>;
        
        public:
        iterator(BSTreeNode<T>* n = 0,BSTreeNode<T>* p = 0): _node(n), _prev(p){}
        iterator(const iterator& i): _node(i._node) , _prev(i._prev) {}
        ~iterator() {}

        const T& operator * () const { return (*this); }
        T& operator * () { return _node->_data; }
        iterator& operator ++ () {
            if(Successor(_node)==0) { _prev=_node; _node=0;}
            else _node=Successor(_node);
            return (*this);
        }
        iterator operator ++ (int) { iterator temp = *this; ++(*this); return temp; }
        iterator& operator -- () {
            //if end()
            if(_node==0 && _prev!=0) { _node=_prev; _prev=0;}
            else _node=Predecessor(_node);
            return *(this);
        }
        iterator operator -- (int) { iterator temp = *this; --(*this); return temp; }
        
        iterator& operator = (const iterator& i) { _node=i._node; return *(this); }
        
        bool operator != (const iterator& i) const { return _node!=i._node; }
        bool operator == (const iterator& i) const { return _node==i._node; }
        
        
        BSTreeNode<T>* min(BSTreeNode<T>* current) const{
            if(current==0) return current;
            while(current->_left!=0){
                current=current->_left;
            }
            return current;
        }
        BSTreeNode<T>* max(BSTreeNode<T>* current) const{
            if(current==0) return current;
            while(current->_right!=0){
                current=current->_right;
            }
            return current;
        }
        BSTreeNode<T>* Predecessor(BSTreeNode<T>* current) const {
            if(current->_left!=0){
                return max(current->_left);
            }
            BSTreeNode<T>* predecessor = current->_parent;
            
            while (predecessor != 0 && current == predecessor->_left) {
                current = predecessor;
                predecessor = predecessor->_parent;
            }
            
            return predecessor;
        }
        BSTreeNode<T>* Successor(BSTreeNode<T>* current) const {
            if(current->_right!=0){
                return min(current->_right);
            }
            BSTreeNode<T>* successor = current->_parent;
            
            while (successor != 0 && current == successor->_right) {
                current = successor;
                successor = successor->_parent;
            }
            return successor;
        }
        
        private:
        BSTreeNode<T>*    _node;
        BSTreeNode<T>*    _prev;
    };
    
    iterator begin() const {
        if(empty()) return end();
        return iterator(min(_root),0);
    }
    iterator end() const {
        return iterator(0,max(_root));
    }
    bool empty() const { return(_root==0);}
    size_t size() const {
        if (empty()) return 0;
        size_t n = 0;
        for (iterator li = begin(); li != end(); li++) {
            n++;
        }
        return n;
    }
    void insert_pos(BSTreeNode<T>* i, const T& x){
        if(x<i->_data){
            if(i->_left==0){
                i->_left=new BSTreeNode<T>(x,0,0,i);
                return;
            }
            else {
                insert_pos(i->_left,x);
            }
        }
        else{
            if(i->_right==0){
                i->_right=new BSTreeNode<T>(x,0,0,i);
                return;
            }
            else {
                insert_pos(i->_right,x);
            }
        }
    }
    void insert(const T& x){
        if(_root==0){
            _root=new BSTreeNode<T>(x);
        }
        else{
            insert_pos(_root,x);
        }
    }
    void pop_front(){
        if(empty()) return;
        erase(begin());
    }
    void pop_back(){
        if(empty()) return;
        erase(--end());
    }
    
    bool erase(iterator pos){
        if(empty()) return false;
        BSTreeNode<T>* del=pos._node;
        //cout<<"delete: "<<*pos<<endl;
        if(del->_left==0 && del->_right==0){
            //cout<<"case1: no child"<<endl;
            if(del->_parent==0){
                delete _root;
                _root=0;
                return true;
            }
            else{
                if(del->_parent->_left==del) del->_parent->_left=0;
                else del->_parent->_right=0;
                del=0;
                return true;
            }
        }
        else if(del->_left==0 || del->_right==0){
            //cout<<"case2: one child";
            if(del->_right==0){
                //cout<<"(has left child)"<<endl;
                if(del->_parent==0){
                    _root=del->_left;
                    _root->_parent=0;
                }
                else{
                    if(del->_parent->_left==del) {
                        del->_left->_parent=del->_parent;
                        del->_parent->_left=del->_left;
                    }
                    else {
                        del->_left->_parent=del->_parent;
                        del->_parent->_right=del->_left;
                    }
                }
            }
            if(del->_left==0){
                //cout<<"(has right child)"<<endl;
                if(del->_parent==0){
                    _root=del->_right;
                    _root->_parent=0;
                }
                else{
                    if(del->_parent->_left==del) {
                        del->_right->_parent=del->_parent;
                        del->_parent->_left=del->_right;
                    }
                    else {
                        del->_right->_parent=del->_parent;
                        del->_parent->_right=del->_right;
                    }
                }
            }
            delete del;
            del=0;
            return true;
        }
        else if(del->_left!=0 && del->_right!=0){
            //cout<<"case3: two children"<<endl;
            BSTreeNode<T>* t=Successor(del);
            if(Successor(del)->_right==0){
                if(Successor(del)==del->_right){
                    del->_data=t->_data;
                    del->_right=0;
                }
                else{
                    del->_data=t->_data;
                    t->_parent->_left=0;
                }
            }
            else{
                if(Successor(del)==del->_right){
                    del->_data=t->_data;
                    del->_right=t->_right;
                    t->_right->_parent=del;
                    t->_right=0;
                }
                else{
                    del->_data=t->_data;
                    t->_parent->_left=t->_right;
                    t->_right->_parent=t->_parent;
                    t->_right=0;
                }
            }
            t->_parent=0;
            delete t;
            t=0;
            return true;
        }
        return false;
    }
    bool erase(const T& x){
        if(find(x)==end()) return false;
        else {
            erase(find(x));
            return true;
        }
    }
    void sort() const{}
    
    void print() const {}
    
    iterator find(const T& x){
        if(empty()) return end();
        BSTreeNode<T>* find=_root;
        while(find!=0 && find->_data!=x){
            if(x<find->_data) find=find->_left;
            else find=find->_right;
        }
        return iterator(find);
    }
    
    BSTreeNode<T>* min(BSTreeNode<T>* current) const {
        if(current==0) return current;
        while(current->_left!=0){
            current=current->_left;
        }
        return current;
    }
    BSTreeNode<T>* max(BSTreeNode<T>* current) const {
        if(current==0) return current;
        while(current->_right!=0){
            current=current->_right;
        }
        return current;
    }
    BSTreeNode<T>* Predecessor(BSTreeNode<T>* current) const {
        if(current->_left!=0){
            return max(current->_left);
        }
        BSTreeNode<T>* predecessor = current->_parent;
        
        while (predecessor != 0 && current == predecessor->_left) {
            current = predecessor;
            predecessor = predecessor->_parent;
        }
        
        return predecessor;
    }
    BSTreeNode<T>* Successor(BSTreeNode<T>* current) const {
        if(current->_right!=0){
            return min(current->_right);
        }
        BSTreeNode<T>* successor = current->_parent;
        
        while (successor != 0 && current == successor->_right) {
            current = successor;
            successor = successor->_parent;
        }
        return successor;
    }
    
    void clear(){
        while(!empty()){ pop_back(); }
        _root=0;
    }
    
    private:
    BSTreeNode<T>* _root;
    
    

};

#endif // BST_H
