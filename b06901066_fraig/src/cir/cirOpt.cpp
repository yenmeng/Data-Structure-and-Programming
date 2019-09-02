/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir optimization functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Please keep "CirMgr::sweep()" and "CirMgr::optimize()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/**************************************************/
/*   Public member functions about optimization   */
/**************************************************/
// Remove unused gates
// DFS list should NOT be changed
// UNDEF, float and unused list may be changed
void
CirMgr::sweep()
{
    DFSlist.clear();
    getDFSlist();
    vector<CirGate*> sweeplist;
    for(unsigned i=0;i<M+O+1;++i){
        if(_gatelist[i]!=0 && !_gatelist[i]->isGlobalRef()){
            if(_gatelist[i]->getTypeStr()=="AIG" || _gatelist[i]->getTypeStr()=="UNDEF"){
                sweeplist.push_back(_gatelist[i]);
            }
        }
    }
    for(unsigned i=0;i<sweeplist.size();++i){
        cout<<"Sweeping: ";
        cout<<sweeplist[i]->getTypeStr()<<"("<<sweeplist[i]->getID()<<") removed..."<<endl;
        removeGate(sweeplist[i]);
    }
    updateFloat();
    getDFSlist();
}

// Recursively simplifying from POs;
// _dfsList needs to be reconstructed afterwards
// UNDEF gates may be delete if its fanout becomes empty...
void
CirMgr::optimize()
{
    DFSlist.clear();
    getDFSlist();
    for(unsigned i=0;i<DFSlist.size();++i){
        if(DFSlist[i]->getTypeStr()!="AIG") continue;
        //if one of the fanins is const0
        else if(DFSlist[i]->faninID[0]==0 || DFSlist[i]->faninID[1]==0){
            //cout<<"case1" <<endl;
            cout<<"Simplifying: ";
            mergeGate(0,DFSlist[i]->getID(),false);
            --A;
            continue;
        }
        //if one of the fanins is const1
        else if(DFSlist[i]->faninID[0]==1 || DFSlist[i]->faninID[1]==1){
            //cout<<"case2" <<endl;
            bool inv=false;
            if(DFSlist[i]->faninID[0]==1 && DFSlist[i]->faninID[1]==1){
                cout<<"Simplifying: ";
                mergeGate(0,DFSlist[i]->getID(),true);
            }
            if(DFSlist[i]->faninID[0]==1 && DFSlist[i]->faninID[1]!=1){
                if(DFSlist[i]->faninID[1]%2!=0) inv=true;
                cout<<"Simplifying: ";
                mergeGate(DFSlist[i]->faninID[1]/2,DFSlist[i]->getID(),inv);
            }
            if(DFSlist[i]->faninID[0]!=1 && DFSlist[i]->faninID[1]==1){
                if(DFSlist[i]->faninID[0]%2!=0) inv=true;
                cout<<"Simplifying: ";
                mergeGate(DFSlist[i]->faninID[0]/2,DFSlist[i]->getID(),inv);
            }
            --A;
        }
        //two fanins are the same
        else if(DFSlist[i]->faninlist[0]->getID()==DFSlist[i]->faninlist[1]->getID()){
            //cout<<"case3" <<endl;
            bool inv=false;
            if(DFSlist[i]->faninID[0]!=DFSlist[i]->faninID[1]) {
                cout<<"Simplifying: ";
                mergeGate(0,DFSlist[i]->getID(),inv);
            }
            else {
                if(DFSlist[i]->faninID[0]%2!=0) inv=true;
                cout<<"Simplifying: ";
                mergeGate(DFSlist[i]->faninID[0]/2,DFSlist[i]->getID(),inv);
            }
            --A;
        }
    }
    updateFloat();
    getDFSlist();
}

/***************************************************/
/*   Private member functions about optimization   */
/***************************************************/
void
CirMgr::removeGate(CirGate* g)
{
    //remove fanin
    for(unsigned i=0;i<g->faninlist.size();++i){
        if(g->faninlist[i]!=0){
            for(unsigned j=0;j<g->faninlist[i]->fanoutlist.size();++j){
                if(g->faninlist[i]->fanoutlist[j]==g){
                    g->faninlist[i]->fanoutlist[j]=0;
                    g->faninlist[i]->fanoutlist.erase(g->faninlist[i]->fanoutlist.begin()+j);
                    g->faninlist[i]->fanoutID.erase(g->faninlist[i]->fanoutID.begin()+j);
                    break;
                }
            }
        }
    }
    //remove fanout
    for(unsigned i=0;i<g->fanoutlist.size();++i){
        if(g->fanoutlist[i]!=0){
            for(unsigned j=0;i<g->fanoutlist[i]->faninlist.size();++j){
                if(g->fanoutlist[i]->faninlist[j]==g){
                    g->fanoutlist[i]->faninlist[j]=0;
                    g->fanoutlist[i]->faninlist.erase(g->fanoutlist[i]->faninlist.begin()+j);
                    g->fanoutlist[i]->faninID.erase(g->fanoutlist[i]->faninID.begin()+j);
                    break;
                }
            }
        }
    }
    if(g->getTypeStr()=="AIG") --A;
    _gatelist[g->getID()]=0;
}
void
CirMgr::mergeGate(unsigned a, unsigned b, bool inv)
{
    cout<<a<<" merging ";
    if(inv) cout<<'!';
    cout<<b<<"..."<<endl;
    for(unsigned i=0;i<_gatelist[b]->faninlist.size();++i){
        if(_gatelist[b]->faninlist[i]!=0){
            for(unsigned j=0;j<_gatelist[b]->faninlist[i]->fanoutlist.size();++j){
                if(_gatelist[b]->faninlist[i]->fanoutlist[j]==_gatelist[b]){
                    _gatelist[b]->faninlist[i]->fanoutlist[j]=0;
                    _gatelist[b]->faninlist[i]->fanoutlist.erase(_gatelist[b]->faninlist[i]->fanoutlist.begin()+j);
                    _gatelist[b]->faninlist[i]->fanoutID.erase(_gatelist[b]->faninlist[i]->fanoutID.begin()+j);
                }
                    
            }
        }
    }
    for(unsigned k=0;k<_gatelist[b]->fanoutlist.size();++k){
        _gatelist[a]->fanoutlist.push_back(_gatelist[b]->fanoutlist[k]);
        bool neg=false;
        if(_gatelist[b]->fanoutID[k]%2!=0) neg=true;
        if((inv && !neg) || (!inv && neg)){ _gatelist[a]->fanoutID.push_back(2*(_gatelist[b]->fanoutID[k]/2)+1);}
        else {_gatelist[a]->fanoutID.push_back(2*(_gatelist[b]->fanoutID[k]/2));}
    }
    for(unsigned i=0;i<_gatelist[b]->fanoutlist.size();++i){
        if(_gatelist[b]->fanoutlist[i]!=0){
            for(unsigned j=0;j<_gatelist[b]->fanoutlist[i]->faninID.size();++j){
                if(_gatelist[b]->fanoutlist[i]->faninlist[j]==_gatelist[b]){
                    bool neg=false;
                    if(_gatelist[b]->fanoutlist[i]->faninID[j]%2!=0) neg=true;
                    _gatelist[b]->fanoutlist[i]->faninlist[j]=0;
                    if((inv && !neg) || (!inv && neg)) { _gatelist[b]->fanoutlist[i]->faninID[j]=(2*a+1); }
                    else { _gatelist[b]->fanoutlist[i]->faninID[j]=(2*a); }
                    _gatelist[b]->fanoutlist[i]->faninlist[j]=_gatelist[a];
                }
            }
        }
    }
    _gatelist[b]=0;
}
