/****************************************************************************
  FileName     [ cirFraig.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir FRAIG functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2012-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "sat.h"
#include "myHashMap.h"
#include "util.h"

using namespace std;

// TODO: Please keep "CirMgr::strash()" and "CirMgr::fraig()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/*******************************************/
/*   Public member functions about fraig   */
/*******************************************/
// _floatList may be changed.
// _unusedList and _undefList won't be changed
class StrashKey
{
public:
    StrashKey() {}
    StrashKey(CirGate* g) {
        in0 = (size_t)g->faninID[0];
        in1 = (size_t)g->faninID[1];
    }
    ~StrashKey() {}
    size_t operator() () const { return (in0 + in1); }
    void operator = (const StrashKey& k) {
        in0 = k.in0;
        in1 = k.in1;
    }
    bool operator == (const StrashKey& k) const {
        return ((in0 == k.in0 && in1 == k.in1)||(in1 == k.in0 && in0 == k.in1));
    }
 
private:
    size_t in0;
    size_t in1;
};

void
CirMgr::strash()
{
    getDFSlist();
    HashMap<StrashKey,unsigned>* cirStrash=new HashMap<StrashKey,unsigned>(getHashSize(DFSlist.size()));
    for(unsigned i=0;i<DFSlist.size();++i){
        if(DFSlist[i]->getTypeStr()=="AIG"){
            StrashKey k(DFSlist[i]);
            unsigned gate=DFSlist[i]->getID();
            if(cirStrash->query(k,gate)){
                cout<<"Strashing: ";
                mergeGate(gate,DFSlist[i]->getID(),false);
                --A;
            }
            else { if(cirStrash->insert(k,gate)){} }
        }
    }
    delete cirStrash;
    cirStrash=0;
    getDFSlist();
}

void
CirMgr::fraig()
{
}

/********************************************/
/*   Private member functions about fraig   */
/********************************************/
