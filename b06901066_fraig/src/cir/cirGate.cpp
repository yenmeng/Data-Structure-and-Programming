/****************************************************************************
 FileName     [ cirGate.cpp ]
 PackageName  [ cir ]
 Synopsis     [ Define class CirAigGate member functions ]
 Author       [ Chung-Yang (Ric) Huang ]
 Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
 ****************************************************************************/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdarg.h>
#include <cassert>
#include "cirGate.h"
#include "cirMgr.h"
#include "util.h"

using namespace std;

extern CirMgr *cirMgr;
unsigned CirGate::_globalRef = 0;
// TODO: Implement memeber functions for class(es) in cirGate.h

/**************************************/
/*   class CirGate member functions   */
/**************************************/
void
CirGate::reportGate() const
{
    cout << "================================================================================" << endl;
    ostringstream os;
    os<<"= ";
    os<<getTypeStr()<<"("<<getID()<<")";
    if(this->_name!="") os<<"\""<<this->_name<<"\"";
    os<<", line "<<_lineNo;
    cout << os.str() << endl;
    cout<<"= FECs:";
    for(unsigned i=0;i<fec.size();++i){
        cout<<' '<<((fec[i]%2==0)? "":"!")<<fec[i]/2;
    }
    cout<<endl;
    cout<<"= Value: ";
    for(unsigned i=0;i<64;++i){
        if(i!=0 && i%8==0) cout<<'_';
        cout<<SimOutStr[i];
    }
    cout<<endl;
    cout << "================================================================================" << endl;
}

void
CirGate::reportFanin(int level)
{
    assert (level >= 0);
    CirGate::setGlobalRef();
    getFaninReport(0,level);
    
}

void
CirGate::reportFanout(int level)
{
    assert (level >= 0);
    CirGate::setGlobalRef();
    getFanoutReport(0,level);
}

void
CirGate:: getFaninReport(int curlevel, int level)
{
    cout<<this->getTypeStr()<<" "<<this->getID();
    if(curlevel==level) { cout<<endl;  return; }
    else{
        if(this->faninlist.empty()) { cout<<endl; this->setToGlobalRef();return; }
        if(this->isGlobalRef()) { cout<< " (*)" <<endl;}
        else {
            cout << endl;
            for(unsigned i=0;i<this->faninlist.size();++i){
                for(int i=0;i<curlevel+1;i++){ cout<<"  " ;}
                if(this->faninID[i]%2==1) cout<<'!';
                this->faninlist[i]->getFaninReport(curlevel+1, level);
            }
        }
    }
    this->setToGlobalRef();
}
void
CirGate:: getFanoutReport(int curlevel, int level)
{
    cout<<this->getTypeStr()<<" "<<this->getID();
    if(curlevel==level) { cout<<endl; return; }
    else{
        if(this->fanoutlist.empty()) { cout<<endl; return; }
        if(this->isGlobalRef()) { cout<< " (*)" <<endl;}
        else {
            cout << endl;
            for(unsigned i=0;i<this->fanoutlist.size();++i){
                for(int i=0;i<curlevel+1;i++){ cout<<"  " ;}
                if(this->fanoutID[i]%2==1) cout<<'!';
                this->fanoutlist[i]->getFanoutReport(curlevel+1, level);
            }
        }
    }
    this->setToGlobalRef();
}


void
ConstGate::printGate()
{
    cout<<"CONST0";
}

void
UndefGate::printGate()
{
    cout<<"UNDEF ";
    cout<<getID();
}

void
PIGate::printGate()
{
    cout<<"PI  ";
    cout<<getID();
    if(getname()!="") {cout<<' '<<"("<<getname()<<")";}
}

void
POGate::printGate()
{
    cout<<"PO  ";
    cout<<getID()<<" ";
    for(unsigned i=0;i<faninlist.size();i++){
        if(faninlist[i]->getTypeStr()=="UNDEF") {cout<<'*';}
        if(faninID[i]%2==1) {cout<<'!';}
        cout<<faninlist[i]->getID();
    }
    if(getname()!="") {cout<<' '<<"("<<getname()<<")";}
}

void
AIGGate::printGate()
{
    cout<<"AIG ";
    cout<<getID()<<' ';
    for(unsigned i=0;i<faninlist.size();i++){
        if(faninlist[i]->getTypeStr()=="UNDEF") {cout<<'*';}
        if(i==1) cout<<' ';
        if(faninID[i]%2==1) {cout<<'!';}
        cout<<faninlist[i]->getID();
    }
}

void
POGate::gateSim()
{
    size_t in0=this->faninlist[0]->getSimVal();
    if(this->faninID[0]%2!=0) in0=~(in0);
    this->setSimVal(in0);
    string ss=bitset<64>(getSimVal()).to_string();
    reverse(ss.begin(),ss.end());
    this->setSimOutStr(ss);
}

void
AIGGate::gateSim()
{
    size_t in0=this->faninlist[0]->getSimVal();
    size_t in1=this->faninlist[1]->getSimVal();
    if(this->faninID[0]%2!=0) in0=~(in0);
    if(this->faninID[1]%2!=0) in1=~(in1);
    size_t o= in0 & in1 ;
    this->setSimVal(o);
    string ss=bitset<64>(getSimVal()).to_string();
    reverse(ss.begin(),ss.end());
    this->setSimOutStr(ss);
}
