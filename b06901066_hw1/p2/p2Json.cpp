/****************************************************************************
  FileName     [ p2Json.cpp ]
  PackageName  [ p2 ]
  Synopsis     [ Define member functions of class Json and JsonElem ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2018-present DVLab, GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include "p2Json.h"

using namespace std;

// Implement member functions of class Row and Table here

bool Json::read(const string& jsonFile)
{
    string line;
    ifstream jf(jsonFile);
    if(jf.is_open())
    {
        while(getline(jf,line)){
            stringstream ss(line);
            string word;
            string key;
            int value=0;;
            int cnt=0;
            
            while(ss>>word){
                cnt++;
                if(word =="{" || word =="}") continue;
                else if(cnt==1)
                {
                    key=word;
                    key.erase(key.begin());
                    key.resize (key.size () - 1);
                }
                
                else if(cnt==3)
                {
                    //cout<<word<<endl;
                    stringstream num(word);
                    num>>value;
            
                    JsonElem j(key,value);
                    _obj.push_back(j);
                }
            }
        }
        return true;
    }
    else return false;
}

void Json::print()
{
    cout<<"{"<<endl;
    for(vector<JsonElem>::iterator it=_obj.begin() ; it != _obj.end(); ++it)
    {
        if(it!=_obj.begin()) {cout<<','<<endl;}
        cout<<"  "<<*it;
    }
    if(_obj.size()!=0)cout<<endl;
    cout<<"}"<<endl;
}
void Json::ave()
{
    if(_obj.size()==0){
    cout<<"Error: No element found!!"<<endl;
    }
    else{
    float total=0;
    for(int i=0 ; i < _obj.size(); i++)
    {
        total+=_obj[i].getvalue();
    }
        total/=_obj.size();
        cout<< "The average of the values is: "<<fixed<<setprecision(1)<<total<<'.'<<endl;
    }
}
void Json::sum()
{
    if(_obj.size()==0){
        cout<<"Error: No element found!!"<<endl;
    }
    else{
    int total=0;
    for(int i=0 ; i < _obj.size(); i++)
    {
        total+=_obj[i].getvalue();
    }
         cout<< "The summation of the values is: "<<total<<'.'<<endl;
    }
}
void Json::min()
{
    if(_obj.size()==0){
        cout<<"Error: No element found!!"<<endl;
    }
    else{
    JsonElem temp=_obj[0];
    for(int i=0 ; i < _obj.size(); i++)
    {
        if(_obj[i].getvalue()<temp.getvalue()) temp=_obj[i];
    }
        cout<<"The minimum element is: { "<<temp<<" }"<<'.'<<endl;
    }
}
void Json::max()
{
    if(_obj.size()==0){
        cout<<"Error: No element found!!"<<endl;
    }
    else{
    JsonElem temp=_obj[0];
    for(int i=0 ; i < _obj.size(); i++)
    {
        if(_obj[i].getvalue()>temp.getvalue()) temp=_obj[i];
    }
        cout<<"The maximum element is: { "<<temp<<" }"<<'.'<<endl;
    }
}
void Json::add(JsonElem& J)
{
    _obj.push_back(J);
}

ostream&
operator << (ostream& os, const JsonElem& j)
{
   return (os << "\"" << j._key << "\" : " << j._value);
} 




