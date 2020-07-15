/****************************************************************************
  FileName     [ dbJson.cpp ]
  PackageName  [ db ]
  Synopsis     [ Define database Json member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2015-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iomanip>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cassert>
#include <climits>
#include <cmath>
#include <string>
#include <algorithm>
#include "dbJson.h"
#include "util.h"

using namespace std;

/*****************************************/
/*          Global Functions             */
/*****************************************/
ostream&
operator << (ostream& os, const DBJsonElem& j)
{
   os << "\"" << j._key << "\" : " << j._value;
   return os;
}

istream& operator >> (istream& is, DBJson& j)
{
   // TODO: to read in data from Json file and store them in a DB 
   // - You can assume the input file is with correct JSON file format
   // - NO NEED to handle error file format
    assert(j._obj.empty());
    assert(j.isread==false);
    while(is.peek()!=EOF){
        string line;
	getline(is,line);
            stringstream ss(line);
            string word;
            string key;
            int value=0;;
            int cnt=0;
            
            while(ss>>word){
                cnt++;
                if(word =="{") continue;
		if(word =="}") break;
                else if(cnt==1)
                {
                    key=word;
                    key.erase(key.begin());
                    key.resize (key.size () - 1);
                }
                
                else if(cnt==3)
                {
                    stringstream num(word);
                    num>>value;
                    
                    DBJsonElem _j(key,value);
                    j._obj.push_back(_j);
                }
            }
        }
    j.isread=true;
    return is;
}

ostream& operator << (ostream& os, const DBJson& j)
{
   // TODO
    os<<"{"<<endl;
    for(long unsigned int i=0 ; i < j._obj.size(); i++)
    {
        if(i!=0) {os<<','<<endl;}
        os<<"  "<<j._obj[i];
    }
    if(j._obj.size()!=0) {os<<endl;}
    os<<"}"<<endl;
    return os;
}

/**********************************************/
/*   Member Functions for class DBJsonElem    */
/**********************************************/
/*****************************************/
/*   Member Functions for class DBJson   */
/*****************************************/
void
DBJson::reset()
{
   // TODO
    while(!_obj.empty())
    {
        _obj.pop_back();
    }
            isread=false;
}

// return false if key is repeated
bool
DBJson::add(const DBJsonElem& elm)
{
   // TODO
    for(long unsigned int i=0 ; i < _obj.size(); i++)
    {
        if(_obj[i].key()==elm.key()) return false;
    }
    _obj.push_back(elm);
   return true;
}

// return NAN if DBJson is empty
float
DBJson::ave() const
{
   // TODO
    if(!_obj.empty()){
        float a = 0.0;
        for(long unsigned int i=0 ; i < _obj.size(); i++)
        {
            a+=_obj[i].value();
        }
        a/=_obj.size();
        return a;
    }
    else return NAN;
}

// If DBJson is empty, set idx to size() and return INT_MIN
int
DBJson::max(size_t& idx) const
{
   // TODO
    if(!_obj.empty()){
        int temp=INT_MIN;
        for(int i=0 ; i < _obj.size(); i++)
        {
            if(_obj[i].value()>temp)
            {
                temp=_obj[i].value();
                idx=i;
            }
        }
        return idx;
    }
        idx=_obj.size();
        int maxN = INT_MIN;
        return  maxN;
}

// If DBJson is empty, set idx to size() and return INT_MIN
int
DBJson::min(size_t& idx) const
{
   // TODO
    if(!_obj.empty()){
        int temp=INT_MAX;
        for(int i=0 ; i < _obj.size(); i++)
        {
            if(_obj[i].value()<temp)
            {
                temp=_obj[i].value();
                idx=i;
            }
        }
        return idx;
    }
        idx=_obj.size();
        int minN = INT_MAX;
        return  minN;
}

void
DBJson::sort(const DBSortKey& s)
{
   // Sort the data according to the order of columns in 's'
   ::sort(_obj.begin(), _obj.end(), s);
}

void
DBJson::sort(const DBSortValue& s)
{
   // Sort the data according to the order of columns in 's'
   ::sort(_obj.begin(), _obj.end(), s);
}

// return 0 if empty
int
DBJson::sum() const
{
   // TODO
    if(!_obj.empty()){
        int s = 0;
        for(long unsigned int i=0 ; i < _obj.size(); i++)
        {
            s+=_obj[i].value();
        }
            return s;
    }
    else return 0;
}
