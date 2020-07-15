/****************************************************************************
  FileName     [ myMinHeap.h ]
  PackageName  [ util ]
  Synopsis     [ Define MinHeap ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_MIN_HEAP_H
#define MY_MIN_HEAP_H

#include <algorithm>
#include <vector>

template <class Data>
class MinHeap
{
public:
   MinHeap(size_t s = 0) { if (s != 0) _data.reserve(s); }
   ~MinHeap() {}

   void clear() { _data.clear(); }

   // For the following member functions,
   // We don't respond for the case vector "_data" is empty!
   const Data& operator [] (size_t i) const { return _data[i]; }   
   Data& operator [] (size_t i) { return _data[i]; }

   size_t size() const { return _data.size(); }

   // TODO
   const Data& min() const { return _data[0]; }
   void insert(const Data& d) {
       _data.push_back(d);
       size_t p = size()-1;
       size_t t = (p-1)/2;
       while (p>0 && d < _data[t]) {
           _data[p]=_data[t];
           p=t;
           t=(p-1)/2;
       }
       _data[p]=d;
   }
   void delMin() {
       delData(0);
   }
   void delData(size_t i) {
       _data[i]=_data.back();
       _data.pop_back();
       //heap down
       if(2*i+1<=size()-1){
           size_t p=i;
           size_t t = (2 * p)+1;
           while (t <= size()-1) {
                if (t < size()-1) {
                    if (_data[t+1] < _data[t] ) ++t;
                    if (_data[p].getLoad() <= _data[t].getLoad()) break;
                }
                else if(t==size()-1 && _data[p].getLoad() <= _data[t].getLoad()) break;
                Data temp=_data[p];
                _data[p]=_data[t];
                _data[t]=temp;
                p = t;
                t = (2 * p)+1;
           }
       }
       //heap up
       else{
           size_t p=i;
           size_t t=(p-1)/2;
           while(p>0 && _data[p]<_data[t]){
               cout<<_data[p]<<endl;
               Data temp=_data[p];
               _data[p]=_data[t];
               _data[t]=temp;
               p=t;
               t=(p-1)/2;
           }
       }
    }

private:
   // DO NOT add or change data members
   vector<Data>   _data;
};

#endif // MY_MIN_HEAP_H
