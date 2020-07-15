/****************************************************************************
  FileName     [ myHashSet.h ]
  PackageName  [ util ]
  Synopsis     [ Define HashSet ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_HASH_SET_H
#define MY_HASH_SET_H

#include <vector>

using namespace std;

//---------------------
// Define HashSet class
//---------------------
// To use HashSet ADT,
// the class "Data" should at least overload the "()" and "==" operators.
//
// "operator ()" is to generate the hash key (size_t)
// that will be % by _numBuckets to get the bucket number.
// ==> See "bucketNum()"
//
// "operator ==" is to check whether there has already been
// an equivalent "Data" object in the HashSet.
// Note that HashSet does not allow equivalent nodes to be inserted
//
template <class Data>
class HashSet
{
public:
   HashSet(size_t b = 0) : _numBuckets(0), _buckets(0) { if (b != 0) init(b); }
   ~HashSet() { reset(); }

   // TODO: implement the HashSet<Data>::iterator
   // o An iterator should be able to go through all the valid Data
   //   in the Hash
   // o Functions to be implemented:
   //   - constructor(s), destructor
   //   - operator '*': return the HashNode
   //   - ++/--iterator, iterator++/--
   //   - operators '=', '==', !="
   //
   class iterator
   {
      friend class HashSet<Data>;

   public:
       iterator(vector<Data>* b=0, size_t idx = 0, vector<Data>* e=0):
       _bucket(b), _idx(idx),  _endbucket(e) {
           if(_idx==0 && _bucket==_endbucket) _node=0;
           else _node=&(*_bucket)[_idx];
       }
       iterator(const iterator& i) : _bucket(i._bucket), _idx(i._idx), _endbucket(i._endbucket)
       {
           if(_idx==0 && _bucket==_endbucket) _node=0;
           else _node=&(*_bucket)[_idx];
       }
       ~iterator() {}
       Data& operator *() { return *_node; }
       const Data& operator * () const { return *_node; }
       iterator& operator ++ () {
           if(_idx<_bucket->size()-1) ++_idx;
           else if(_idx==_bucket->size()-1){
               _idx=0;
               while(++_bucket!=_endbucket){ if(!_bucket->empty()) break;}
           }
           if(_idx==0 && _bucket==_endbucket) _node=0;
           else _node=&(*_bucket)[_idx];
           return *(this);
       }
       iterator operator ++ (int) { iterator temp = *this; ++(*this); return temp; }
       iterator& operator -- () {
           if(_idx>0) --_idx;
           else if(_idx==0 && !_bucket->empty()){
               --_bucket;
               while(_bucket->empty()) { --_bucket; }
               _idx=_bucket->size()-1;
           }
           _node=&(*_bucket)[_idx];
           return *(this);
       }
       iterator operator -- (int) { iterator temp = *this; --(*this); return temp; }
       iterator& operator = (const iterator& i) {
           _bucket=i._bucket;
           _idx=i._idx;
           _endbucket=i._endbucket;
           _node=i._node;
           return *(this);
       }
       bool operator == (const iterator& i) const { return (_node == i._node); }
       bool operator != (const iterator& i) const { return (_node != i._node); }
   private:
       vector<Data>*  _bucket;
       size_t _idx;
       vector<Data>*  _endbucket;
       Data* _node;
   };

   void init(size_t b) { _numBuckets = b; _buckets = new vector<Data>[b]; }
   void reset() {
      _numBuckets = 0;
      if (_buckets) { delete [] _buckets; _buckets = 0; }
   }
   void clear() {
      for (size_t i = 0; i < _numBuckets; ++i) _buckets[i].clear();
   }
   size_t numBuckets() const { return _numBuckets; }

   vector<Data>& operator [] (size_t i) { return _buckets[i]; }
   const vector<Data>& operator [](size_t i) const { return _buckets[i]; }

   // TODO: implement these functions
   //
   // Point to the first valid data
   iterator begin() const {
       for(size_t i = 0;i<_numBuckets; ++i){
           if(!(_buckets+i)->empty()) return iterator(_buckets+i,0,_buckets+_numBuckets);
       }
       return end();
   }
   // Pass the end
   iterator end() const { return iterator(_buckets+_numBuckets,0,_buckets+_numBuckets); }
   // return true if no valid data
   bool empty() const { return (begin()==end()); }
   // number of valid data
   size_t size() const {
       size_t s = 0;
       for(size_t i=0;i<_numBuckets;++i){
           s+=_buckets[i].size();
       }
       return s;
   }

   // check if d is in the hash...
   // if yes, return true;
   // else return false;
   bool check(const Data& d) const {
       size_t index=bucketNum(d);
       if(!_buckets[index].empty()){
           for(size_t i=0;i<_buckets[index].size();++i){
               if(d == _buckets[index][i]) return true;
           }
       }
       return false;
   }

   // query if d is in the hash...
   // if yes, replace d with the data in the hash and return true;
   // else return false;
   bool query(Data& d) const {
       size_t index=bucketNum(d);
       if(!_buckets[index].empty()){
           for(size_t i=0;i<_buckets[index].size();i++){
               if(_buckets[index][i] == d) {
                   d=_buckets[index][i];
                   return true;
               }
           }
       }
       return false;
   }

   // update the entry in hash that is equal to d (i.e. == return true)
   // if found, update that entry with d and return true;
   // else insert d into hash as a new entry and return false;
   bool update(const Data& d) {
       size_t index=bucketNum(d);
       for(size_t i=0;i<_buckets[index].size();++i){
           if(_buckets[index][i]==d) {
               _buckets[index][i]=d;
               return true;
           }
       }
       _buckets[index].push_back(d);
       return false;
   }

   // return true if inserted successfully (i.e. d is not in the hash)
   // return false is d is already in the hash ==> will not insert
   bool insert(const Data& d) {
       if(check(d)) return false;
       size_t index=bucketNum(d);
       _buckets[index].push_back(d);
       return true;
   }

   // return true if removed successfully (i.e. d is in the hash)
   // return fasle otherwise (i.e. nothing is removed)
   bool remove(const Data& d) {
       size_t index=bucketNum(d);
       for(size_t i=0;i<_buckets[index].size();i++){
           if(_buckets[index][i]==d){
               _buckets[index].erase(_buckets[index].begin()+i);
                return true;
           }
       }
       return false;
   }

private:
   // Do not add any extra data member
   size_t            _numBuckets;
   vector<Data>*     _buckets;

   size_t bucketNum(const Data& d) const {
      return (d() % _numBuckets); }
};

#endif // MY_HASH_SET_H
