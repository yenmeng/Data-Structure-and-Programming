/****************************************************************************
  FileName     [ cirMgr.h ]
  PackageName  [ cir ]
  Synopsis     [ Define circuit manager ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_MGR_H
#define CIR_MGR_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>

using namespace std;

#include "cirDef.h"

extern CirMgr *cirMgr;

// TODO: Define your own data members and member functions
class CirMgr
{
    static unsigned _globalRef;
public:
    CirMgr(){ _gatelist=0; }
    ~CirMgr() { reset();}

   // Access functions
   // return '0' if "gid" corresponds to an undefined gate.
   CirGate* getGate(unsigned gid) const {
       if(gid>M+O || _gatelist[gid] == 0 ) return 0;
       else return _gatelist[gid];
   }

   // Member functions about circuit construction
   bool readCircuit(const string&);
    void connect();
   // Member functions about circuit reporting
   void printdata() const;
   void printSummary() const;
   void printNetlist() const;
   void printPIs() const;
   void printPOs() const;
   void printFloatGates() const;
    void writeAag(ostream&) const;
   //DFS functions
    void DFStraversal(CirGate*);
    void getDFSlist();
    void reset();
    bool myStr2Unsigned(const string& str, unsigned& num)
    {
        num = 0;
        size_t i = 0;
        bool valid = false;
        for (; i < str.size(); ++i) {
            if (isdigit(str[i])) {
                num *= 10;
                num += int(str[i] - '0');
                valid = true;
            }
            else return false;
        }
        return valid;
    }
private:
    CirGate**  _gatelist;
    unsigned M;
    unsigned I;
    unsigned L;
    unsigned O;
    unsigned A;
    vector<CirGate*> PIlist;
    vector<CirGate*> POlist;
    vector<CirGate*> Floatlist1;
    vector<CirGate*> Floatlist2;
    vector<CirGate*> DFSlist;
    
};

#endif // CIR_MGR_H
