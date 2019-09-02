/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir simulation functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <fstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cassert>
#include <bitset>
#include <sstream>
#include <cmath>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Keep "CirMgr::randimSim()" and "CirMgr::fileSim()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/************************************************/
/*   Public member functions about Simulation   */
/************************************************/
class SimKey{
public:
    SimKey(size_t k) : _simval(k){}
    ~SimKey() {}
    void operator =  (const SimKey& k) { _simval = k._simval; }
    bool operator == (const SimKey& k) const {
        return (_simval == k._simval || _simval == ~k._simval);
    }
    size_t operator() () const {
        return _simval;
    }
private:
    size_t  _simval;
};

void
CirMgr::randomSim()
{
    size_t max_fail=sqrt(A);
    size_t sim_pattern=0;
    fail=0;
    fail_flag=false;
    for(unsigned i=0;i<I;++i){
        _signal.push_back(0);
    }
    while(fail<max_fail && !fail_flag ){
        //genSignal
        for(unsigned i=0;i<I;++i){
            _signal[i]=(rnGen(INT_MAX));
        }
        loadSignal();
        All_Gate_Sim();
        getFECs();
        sort(FEClist.begin(),FEClist.end());
        if(_simLog) { writeSim(64);}
        sim_pattern+=64;
    }
    _signal.clear();
    cout << sim_pattern << " patterns simulated." << endl;
}

void
CirMgr::fileSim(ifstream& patternFile)
{
    _simfile.clear();
    if(!readSimFile(patternFile)) return;
    size_t sim_round=0;
    size_t sim_pattern=_simfile.size();
    if(_simfile.size()<=64) { sim_round=1; }
    else {
        if(_simfile.size()%64==0) sim_round=_simfile.size()/64;
        else sim_round=_simfile.size()/64+1;
    }
    for(size_t i=0 ; i<sim_round ; ++i){
        Stringto64();
        loadSignal();
        All_Gate_Sim();
        getFECs();
        sort(FEClist.begin(),FEClist.end());
        if(_simLog) {
            if(_simfile.size()<64) writeSim(_simfile.size());
            else writeSim(64);
        }
        if(_simfile.size()>64) { _simfile.erase(_simfile.begin(),_simfile.begin()+64); }
        else _simfile.clear();
    }
    cout<<sim_pattern<<" patterns simulated."<<endl;
}

/*************************************************/
/*   Private member functions about Simulation   */
/*************************************************/
bool
CirMgr::readSimFile(ifstream& patternFile)
{
    string buf;
    while(patternFile>>buf){
        if(buf.size()!=PIlist.size()){
            cout << endl;
            cerr << "Error: Pattern(" << buf << ") length(" << buf.size()
            << ") does not match the number of inputs(" << PIlist.size()<< ") in a circuit!!"<<endl;
            cout << "0 patterns simulated." << endl;
            return false;
        }
        if(buf.find_first_not_of("01") != std::string::npos){
            cout << endl;
            cerr << "Error: Pattern(" << buf << ") contains a non-0/1 character('"
            << buf[buf.find_first_not_of("01")] << "')." << endl;
            cout << "0 patterns simulated." << endl;
            return false;
        }
        _simfile.push_back(buf);
    }
    return true;
}
void
CirMgr::Stringto64()
{
    _signal.clear();
    for(size_t i=0; i<I ;i++){
        _signal.push_back(size_t(0));
        if(_simfile.size()<64){
            for(size_t j=0;j<_simfile.size();j++){
                size_t n=_simfile[j][i]-'0';
                _signal[i]+=n<<(63-j);
            }
            
        }
        else{
            for(size_t j=0;j<64;j++){
                size_t n=_simfile[j][i]-'0';
                _signal[i]+=n<<(63-j);
            }
        }
    }
}
void
CirMgr::loadSignal()
{
    for(unsigned i=0; i<PIlist.size(); ++i){
        PIlist[i]->setSimVal(_signal[i]);
        string ss=bitset<64>(PIlist[i]->getSimVal()).to_string();
        reverse(ss.begin(),ss.end());
        PIlist[i]->setSimOutStr(ss);
    }
}
void
CirMgr::All_Gate_Sim()
{
    for(unsigned i=0; i<DFSlist.size(); ++i){
        if(DFSlist[i]->getTypeStr()=="AIG" || DFSlist[i]->getTypeStr()=="PO"){
            DFSlist[i]->gateSim();
        }
    }
}

void
CirMgr::getFECs()
{
    A_hash.clear();
    if(!FEClist.empty()){ updateFEC(); return; }
    HashMap<SimKey,unsigned>* cirSim = new HashMap<SimKey,unsigned>(getHashSize(DFSlist.size()));
    for(unsigned i=0;i<DFSlist.size();++i){
        A_hash.push_back(DFSlist[i]->getID());
    }
    sort(A_hash.begin(),A_hash.end());
    SimKey const0(_gatelist[0]->getSimVal());
    if(cirSim->insert(const0,0)) {}
    for(unsigned i=0;i<A_hash.size();++i){
        if(_gatelist[A_hash[i]]->getTypeStr()=="AIG"){
                unsigned g=_gatelist[A_hash[i]]->getID();
                unsigned temp=g;
            SimKey k(_gatelist[g]->getSimVal());
            SimKey not_k(~_gatelist[g]->getSimVal());
            if(cirSim->query(k,g) || cirSim->query(not_k,g)){
                if(_gatelist[g]->getSimVal()==~_gatelist[temp]->getSimVal())
                    _gatelist[g]->fec.push_back(2*temp+1);
                else _gatelist[g]->fec.push_back(2*temp);
            }
            else { if(cirSim->insert(k,g)) {} }
        }
    }
    getFEClist(cirSim);
    if(FEClist.empty()) fail_flag=true;
    delete cirSim;
    cirSim=0;
    
}
void
CirMgr::getFEClist(HashMap<SimKey,unsigned>* cirSim)
{
    for (HashMap<SimKey, unsigned>::iterator i = cirSim->begin(); i!= cirSim->end(); ++i) {
        if(getGate((*i).second)->fec.size()!=0){
            FEClist.push_back((*i).second);
        }
    }
    for(unsigned i=0;i<FEClist.size();++i){
        sort(getGate(FEClist[i])->fec.begin(),getGate(FEClist[i])->fec.end());
        for(unsigned j=0;j<getGate(FEClist[i])->fec.size();++j){
            getGate(getGate(FEClist[i])->fec[j]/2)->fec=getGate(FEClist[i])->fec;
            getGate(getGate(FEClist[i])->fec[j]/2)->fec.insert(getGate(getGate(FEClist[i])->fec[j]/2)->fec.begin(),2*FEClist[i]);
            if(getGate(FEClist[i])->fec[j]%2!=0){
                for(unsigned k=0;k<getGate(getGate(FEClist[i])->fec[j]/2)->fec.size();++k){
                    if(getGate(getGate(FEClist[i])->fec[j]/2)->fec[k]%2==0)
                        ++getGate(getGate(FEClist[i])->fec[j]/2)->fec[k];
                    else --getGate(getGate(FEClist[i])->fec[j]/2)->fec[k];
                }
            }
            getGate(getGate(FEClist[i])->fec[j]/2)->fec.erase(getGate(getGate(FEClist[i])->fec[j]/2)->fec.begin()+(j+1));
        }
    }
}

void
CirMgr::updateFEC()
{
    bool simfail=true;
    size_t origin=FEClist.size();
    for(unsigned i=0;i<origin;++i){
        new_cirSim.clear();
        new_cirSim.push_back(FEClist[i]);
        //split into small hashes
        for(unsigned j=0;j<getGate(FEClist[i])->fec.size(); ++j){
            new_cirSim.push_back((getGate(FEClist[i])->fec[j]/2));
        }
        resetFECs(FEClist[i]);
        HashMap<SimKey,unsigned>* cirSim = new HashMap<SimKey,unsigned>(getHashSize(new_cirSim.size()));
        for(unsigned l=0;l<new_cirSim.size();++l){
            unsigned g=new_cirSim[l];
            unsigned temp=g;
            SimKey k(_gatelist[g]->getSimVal());
            SimKey not_k(~_gatelist[g]->getSimVal());
            if(cirSim->query(k,g) || cirSim->query(not_k,g)){
                if(_gatelist[g]->getSimVal()==~_gatelist[temp]->getSimVal())
                     _gatelist[g]->fec.push_back(2*temp+1);
                else _gatelist[g]->fec.push_back(2*temp);
            }
            else { if(cirSim->insert(k,g)) {} }
        }
        bool del=true;
        if(getGate(new_cirSim[0])->fec.size()!=0){
            del=false;
            sort(getGate(FEClist[i])->fec.begin(),getGate(FEClist[i])->fec.end());
            for (HashMap<SimKey, unsigned>::iterator it = cirSim->begin(); it!= cirSim->end(); ++it) {
                if(getGate((*it).second)->fec.size()!=0 && (*it).second!=FEClist[i]){
                    simfail=false;
                    FEClist.push_back(((*it).second));
                }
            }
            for(unsigned j=0;j<getGate(FEClist[i])->fec.size();++j){
                getGate(getGate(FEClist[i])->fec[j]/2)->fec=getGate(FEClist[i])->fec;
                getGate(getGate(FEClist[i])->fec[j]/2)->fec.insert(getGate(getGate(FEClist[i])->fec[j]/2)->fec.begin(),2*FEClist[i]);
                if(getGate(FEClist[i])->fec[j]%2!=0){
                    for(unsigned k=0;k<getGate(getGate(FEClist[i])->fec[j]/2)->fec.size();++k){
                        if(getGate(getGate(FEClist[i])->fec[j]/2)->fec[k]%2==0)
                            ++getGate(getGate(FEClist[i])->fec[j]/2)->fec[k];
                        else --getGate(getGate(FEClist[i])->fec[j]/2)->fec[k];
                    }
                }
                getGate(getGate(FEClist[i])->fec[j]/2)->fec.erase(getGate(getGate(FEClist[i])->fec[j]/2)->fec.begin()+(j+1));
            }
        }
        else if(getGate(FEClist[i])->fec.size()==0){
            simfail=false;
            for (HashMap<SimKey, unsigned>::iterator it = cirSim->begin(); it!= cirSim->end(); ++it) {
                if(getGate((*it).second)->fec.size()!=0 && (*it).second!=FEClist[i]){
                    FEClist.push_back(((*it).second));
                }
            }
        }
        for(unsigned n=origin;n<FEClist.size();++n) updateFEClist(FEClist[n]);
        if(del) { FEClist.erase(FEClist.begin()+i); --origin; --i;}
        delete cirSim;
        cirSim=0;
    }
    if(simfail) ++fail;
    if(FEClist.empty()) fail_flag=true;
}

void
CirMgr::updateFEClist(unsigned n)
{
    sort(getGate(n)->fec.begin(),getGate(n)->fec.end());
    for(unsigned i=0;i<getGate(n)->fec.size();++i){
            getGate(getGate(n)->fec[i]/2)->fec=getGate(n)->fec;
            getGate(getGate(n)->fec[i]/2)->fec.insert(getGate(getGate(n)->fec[i]/2)->fec.begin(),2*n);
            if(getGate(n)->fec[i]%2!=0){
                for(unsigned k=0;k<getGate(getGate(n)->fec[i]/2)->fec.size();++k){
                    if(getGate(getGate(n)->fec[i]/2)->fec[k]%2==0)
                        ++getGate(getGate(n)->fec[i]/2)->fec[k];
                    else --getGate(getGate(n)->fec[i]/2)->fec[k];
                }
            }
            getGate(getGate(n)->fec[i]/2)->fec.erase(getGate(getGate(n)->fec[i]/2)->fec.begin()+(i+1));
    }
}

void
CirMgr::resetFECs(unsigned g)
{
    for(unsigned i=0;i<_gatelist[g]->fec.size();++i){
        getGate(_gatelist[g]->fec[i]/2)->fec.clear();
    }
    _gatelist[g]->fec.clear();
}

void
CirMgr::writeSim(unsigned n)
{
    if(n<64){
        for(unsigned i=0;i<n;++i){
            for(unsigned j=0;j<I;j++){
                (*_simLog)<<PIlist[j]->SimOutStr[63-i];
            }
            (*_simLog)<<' ';
            for(unsigned j=0;j<O;j++){
                (*_simLog)<<POlist[j]->SimOutStr[63-i];
            }
            (*_simLog)<<endl;
        }
    }
    else{
        for(unsigned i=0;i<64;++i){
            for(unsigned j=0;j<I;j++){
                (*_simLog)<<PIlist[j]->SimOutStr[63-i];
            }
            (*_simLog)<<' ';
            for(unsigned j=0;j<O;j++){
                (*_simLog)<<POlist[j]->SimOutStr[63-i];
            }
            (*_simLog)<<endl;
        }
    }
}

