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

// TODO: Feel free to define your own classes, variables, or functions.

#include "cirDef.h"

extern CirMgr *cirMgr;
class StrashKey;
class SimKey;

class CirMgr
{
public:
    CirMgr() { _gatelist=0; fail=0; fail_flag=false;}
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
   // Member functions about circuit optimization
   void sweep();
   void optimize();
   void removeGate(CirGate*);
   void mergeGate(unsigned, unsigned, bool);
   void updateFloat();
   // Member functions about simulation
   void randomSim();
   void fileSim(ifstream&);
   void setSimLog(ofstream *logFile) { _simLog = logFile; }
    bool readSimFile(ifstream&);
    void Stringto64();
    void loadSignal();
    void All_Gate_Sim();
    void getFECs();
    void getFEClist(HashMap<SimKey,unsigned>*);
    void updateFEC();
    void updateFEClist(unsigned);
    void resetFECs(unsigned);
    void writeSim(unsigned);
   // Member functions about fraig
   void strash();
   void printFEC() const;
   void fraig();

   // Member functions about circuit reporting
   void printSummary() const;
   void printNetlist() const;
   void printPIs() const;
   void printPOs() const;
   void printFloatGates() const;
   void printFECPairs() const;
   void writeAag(ostream&) const;
   void writeGate(ostream&, CirGate*) const;
    
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
    ofstream   *_simLog;
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
    //for simulation
    vector<string> _simfile;
    vector<size_t> _signal;
    vector<unsigned> FEClist;
    vector<unsigned> A_hash;
    vector<unsigned> new_cirSim;
    size_t fail;
    bool fail_flag;
};

#endif // CIR_MGR_H
