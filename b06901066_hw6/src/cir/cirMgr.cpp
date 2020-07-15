/****************************************************************************
  FileName     [ cirMgr.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir manager functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <cstdio>
#include <ctype.h>
#include <cassert>
#include <cstring>
#include <sstream>
#include <vector>
#include <algorithm>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Implement memeber functions for class CirMgr

/*******************************/
/*   Global variable and enum  */
/*******************************/
CirMgr* cirMgr = 0;
enum CirParseError {
   EXTRA_SPACE,
   MISSING_SPACE,
   ILLEGAL_WSPACE,
   ILLEGAL_NUM,
   ILLEGAL_IDENTIFIER,
   ILLEGAL_SYMBOL_TYPE,
   ILLEGAL_SYMBOL_NAME,
   MISSING_NUM,
   MISSING_IDENTIFIER,
   MISSING_NEWLINE,
   MISSING_DEF,
   CANNOT_INVERTED,
   MAX_LIT_ID,
   REDEF_GATE,
   REDEF_SYMBOLIC_NAME,
   REDEF_CONST,
   NUM_TOO_SMALL,
   NUM_TOO_BIG,

   DUMMY_END
};

/**************************************/
/*   Static varaibles and functions   */
/**************************************/
static unsigned lineNo = 0;  // in printint, lineNo needs to ++
static unsigned colNo  = 0;  // in printing, colNo needs to ++
//static char buf[1024];
static string errMsg;
static int errInt;
static CirGate *errGate;

static bool
parseError(CirParseError err)
{
   switch (err) {
      case EXTRA_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Extra space character is detected!!" << endl;
         break;
      case MISSING_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing space character!!" << endl;
         break;
      case ILLEGAL_WSPACE: // for non-space white space character
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal white space char(" << errInt
              << ") is detected!!" << endl;
         break;
      case ILLEGAL_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal "
              << errMsg << "!!" << endl;
         break;
      case ILLEGAL_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal identifier \""
              << errMsg << "\"!!" << endl;
         break;
      case ILLEGAL_SYMBOL_TYPE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal symbol type (" << errMsg << ")!!" << endl;
         break;
      case ILLEGAL_SYMBOL_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Symbolic name contains un-printable char(" << errInt
              << ")!!" << endl;
         break;
      case MISSING_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing " << errMsg << "!!" << endl;
         break;
      case MISSING_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing \""
              << errMsg << "\"!!" << endl;
         break;
      case MISSING_NEWLINE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": A new line is expected here!!" << endl;
         break;
      case MISSING_DEF:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing " << errMsg
              << " definition!!" << endl;
         break;
      case CANNOT_INVERTED:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": " << errMsg << " " << errInt << "(" << errInt/2
              << ") cannot be inverted!!" << endl;
         break;
      case MAX_LIT_ID:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Literal \"" << errInt << "\" exceeds maximum valid ID!!"
              << endl;
         break;
      case REDEF_GATE:
         cerr << "[ERROR] Line " << lineNo+1 << ": Literal \"" << errInt
              << "\" is redefined, previously defined as "
              << errGate->getTypeStr() << " in line " << errGate->getLineNo()
              << "!!" << endl;
         break;
      case REDEF_SYMBOLIC_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ": Symbolic name for \""
              << errMsg << errInt << "\" is redefined!!" << endl;
         break;
      case REDEF_CONST:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Cannot redefine constant (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_SMALL:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too small (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_BIG:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too big (" << errInt << ")!!" << endl;
         break;
      default: break;
   }
   return false;
}

/**************************************************************/
/*   class CirMgr member functions for circuit construction   */
/**************************************************************/
bool
CirMgr::readCircuit(const string& fileName)
{
    ifstream file(fileName.c_str(),ios::in);
    if(!file.is_open() ) {
        cerr << "Cannot open design \""<<fileName<<"\"!!\n";
        return false;
    }
    //****** header ******
    string line;
    if(!getline(file,line)){
        errMsg = "aag";
        return parseError(MISSING_IDENTIFIER);
    }
    stringstream ss(line);
    string word;
    int cnt=0;
    if(line.find(' ')==0){
        return parseError(EXTRA_SPACE);
    }
    if(line.find('\t')==0){
        errInt='\t';
        return parseError(ILLEGAL_WSPACE);
    }
    while(ss>>word){
        cnt++;
        if(cnt==1){
            if(myStrNCmp("aag",word.substr(0,3),3)!=0){
                errMsg=word;
                return parseError(ILLEGAL_IDENTIFIER);
            }
            if(myStrNCmp("aag",word.substr(0,3),3)==0 && word.size()>3){
                unsigned temp;
                if(myStr2Unsigned(word.substr(3),temp)){
                    colNo=3;
                    return parseError(MISSING_SPACE);
                }
                else{
                    errMsg=word;
                    return parseError(ILLEGAL_IDENTIFIER);
                }
            }
        }
        if(cnt==2){
            if(!myStr2Unsigned(word,M)){
                errMsg = "number of variables(" + word + ")";
                return parseError(ILLEGAL_NUM);
            }
        }
        if(cnt==3){
            if(!myStr2Unsigned(word,I)){
                errMsg = "number of PIs(" + word + ")";
                return parseError(ILLEGAL_NUM);
            }
        }
        if(cnt==4){
            myStr2Unsigned(word,L);
        }
        if(cnt==5){
            if(!myStr2Unsigned(word,O)){
                errMsg = "number of POs(" + word + ")";
                return parseError(ILLEGAL_NUM);
            }
        }
        if(cnt==6){
            if(!myStr2Unsigned(word,A)){
                errMsg = "number of AIGs(" + word + ")";
                return parseError(ILLEGAL_NUM);
            }
        }
    }
    if(L!=0){
        errMsg="latches";
        return parseError(ILLEGAL_NUM);
    }
    if(cnt==1){
        colNo=4;
        errMsg = "number of variables";
        return parseError(MISSING_NUM);
    }
    if(cnt<6){
        colNo=line.size();
        errMsg = "number of variables";
        return parseError(MISSING_NUM);
    }
    if(line[line.size()-1]==' '){
        colNo=line.find_last_not_of(' ')+1;
        return parseError(MISSING_NEWLINE);
    }
    if(M<I+L+A){
        errMsg = "number of variables";
        errInt = M;
        return parseError(NUM_TOO_SMALL);
    }
    lineNo++;
    //****** gatelist ******
    _gatelist = new CirGate*[M+O+1]();
    _gatelist[0] = new ConstGate(0,0);
    //****** I ******
    string line1;
    for(unsigned i=0;i<I;i++){
        unsigned literalID;
        //PIs not enough
        if(!getline(file,line1)){
            errMsg = "PI";
            return parseError(MISSING_DEF);
        }
        //empty PI ID
        if(line1.empty()){
            errMsg = "PI literal ID";
            return parseError(MISSING_NUM);
        }
        //EXTRA leading space
        if(line1.find(' ') == 0) return parseError(EXTRA_SPACE);
        //EXTRA trailing space
        if(line1.find(' ') != string::npos) {
            colNo=line1.find(' ');
            return parseError(MISSING_NEWLINE);
        }
        //ILLEGAL PI ID
        if(!myStr2Unsigned(line1,literalID)){
            errMsg = "PI literal ID(" + line1 + ")";
            return parseError(ILLEGAL_NUM);
        }
        //ID exceeds M
        if(literalID/2>M){
            errInt=literalID;
            return parseError(MAX_LIT_ID);
        }
        //redef const
        if(literalID/2 == 0) {
            errInt=literalID;
            return parseError(REDEF_CONST);
        }
        //cannot be inverted
        if(literalID%2==1){
            errMsg = "PI";
            errInt=literalID;
            return parseError(CANNOT_INVERTED);
        }
        //redef gate
        if(_gatelist[literalID/2]!=0 && _gatelist[literalID/2]->getTypeStr()!="UNDEF"){
            errGate=_gatelist[literalID/2];
            errInt=literalID;
            return parseError(REDEF_GATE);
        }
        lineNo++;
        CirGate* in=new PIGate(literalID/2,lineNo);
        PIlist.push_back(in);
        _gatelist[literalID/2]=in;
    }
    //****** L ******
    string line2;
    //****** O ******
    string line3;
    for(unsigned i=0;i<O;i++){
        unsigned literalID;
        colNo=0;
        //POs not enough
        if(!getline(file,line3)){
            errMsg = "PO";
            return parseError(MISSING_DEF);
        }
        //empty PO ID
        if(line3.empty()){
            errMsg = "PO literal ID";
            return parseError(MISSING_NUM);
        }
        //EXTRA leading space
        if(line3.find(' ') == 0) return parseError(EXTRA_SPACE);
        //EXTRA trailing space
        if(line3.find(' ') != string::npos) {
            colNo=line3.find(' ');
            return parseError(MISSING_NEWLINE);
        }
        //ILLEGAL PO ID
        if(!myStr2Unsigned(line3,literalID)){
            errMsg = "PO literal ID(" + line3 + ")";
            return parseError(ILLEGAL_NUM);
        }
        //ID exceeds M
        if(literalID/2>M){
            errInt=literalID;
            return parseError(MAX_LIT_ID);
        }
        lineNo++;
        CirGate* out=new POGate(M+i+1,lineNo);
        POlist.push_back(out);
        _gatelist[M+i+1]=out;
        if(_gatelist[literalID/2]==0) {
            _gatelist[literalID/2]=new UndefGate(literalID/2,lineNo);
        }
        _gatelist[M+i+1]->faninID.push_back(literalID);
        (literalID%2==0)?_gatelist[literalID/2]->fanoutID.push_back(2*(M+i+1)):_gatelist[literalID/2]->fanoutID.push_back(2*(M+1+i)+1);
    }
    //****** A ******
    string line4;
    for(unsigned i=0;i<A;i++){
        //AIGs not enough
        if(!getline(file,line4)){
            errMsg = "AIG";
            return parseError(MISSING_DEF);
        }
        //empty AIG ID
        if(line4.empty()){
            errMsg = "AIG literal ID";
            return parseError(MISSING_NUM);
        }
        //EXTRA leading space
        if(line4.find(' ') == 0) return parseError(EXTRA_SPACE);
        vector<string> tokens;
        string token;
        size_t n=myStrGetTok(line4, token);
        unsigned c=0;
        unsigned k=0;
        while (token.size()) {
            tokens.push_back(token);
            n = myStrGetTok(line4, token, n);
            unsigned temp;
            myStr2Unsigned(tokens[k],temp);
            if(temp/2>M && tokens.size()<=3){
                if(k==0) colNo=0;
                else colNo=c+1;
                errInt=temp;
                return parseError(MAX_LIT_ID);
            }
            c+=tokens[k].size();
            k++;
            if(tokens.size()>3){
                colNo=c+1;
                return parseError(MISSING_NEWLINE);
            }
        }
        if(line4[line4.size()-1]==' '){
            colNo=c+2;
            return parseError(MISSING_NEWLINE);
        }
        unsigned LHS;
        unsigned RHS1;
        unsigned RHS2;
        myStr2Unsigned(tokens[0],LHS);
        myStr2Unsigned(tokens[1],RHS1);
        myStr2Unsigned(tokens[2],RHS2);
        //redef const
        if(LHS/2 == 0) {
            errInt=LHS;
            colNo=0;
            return parseError(REDEF_CONST);
        }
        //redef gate
        if(_gatelist[LHS/2]!=0 && _gatelist[LHS/2]->getTypeStr()!="UNDEF"){
            errGate=_gatelist[LHS/2];
            errInt=LHS;
            return parseError(REDEF_GATE);
        }
        lineNo++;
        if(_gatelist[RHS1/2]==0){
            CirGate* undef=new UndefGate(RHS1/2,0);
            _gatelist[RHS1/2]=undef;
        }
        if(_gatelist[RHS2/2]==0){
            CirGate* undef=new UndefGate(RHS2/2,0);
            _gatelist[RHS2/2]=undef;
        }
        if(_gatelist[LHS/2]==0){
            CirGate* And=new AIGGate(LHS/2,lineNo);
            _gatelist[LHS/2]=And;
        }
        if(_gatelist[LHS/2]->getTypeStr()=="UNDEF"){
            CirGate* And=new AIGGate(LHS/2,lineNo);
            And->faninID=_gatelist[LHS/2]->faninID;
            And->fanoutID=_gatelist[LHS/2]->fanoutID;
            _gatelist[LHS/2]=And;
        }
        //Set fanin&fanout
        _gatelist[LHS/2]->faninID.push_back(RHS1);
        _gatelist[LHS/2]->faninID.push_back(RHS2);
        (RHS1%2==0)?_gatelist[RHS1/2]->fanoutID.push_back(LHS):_gatelist[RHS1/2]->fanoutID.push_back(LHS+1);
        (RHS2%2==0)?_gatelist[RHS2/2]->fanoutID.push_back(LHS):_gatelist[RHS2/2]->fanoutID.push_back(LHS+1);
        _gatelist[LHS/2]->setlineNo(lineNo);
    }
    connect();
    //****** symbol ******
    string line5;
    while(getline(file,line5)){
        if(line5.empty()){
            errMsg = "symbolic name";
            return parseError(MISSING_IDENTIFIER);
        }
        if(line5[0]=='c'){
            if(line5.size()>1){
                colNo=1;
                return parseError(MISSING_NEWLINE);
            }
            break;
        }
        vector<string> tokens;
        size_t n=line5.find(' ');
        if(n==0){
            colNo=0;
            return parseError(EXTRA_SPACE);
        }
        if(n==string::npos || line5.size()<=n+1){
            errMsg="symbolic name";
            return parseError(MISSING_IDENTIFIER);
        }
        tokens.push_back(line5.substr(0,n));
        tokens.push_back(line5.substr(n+1));
        char type=tokens[0].at(0);
        string idx=tokens[0].substr(1);
        if(type!='i' && type!='o'){
            errMsg = type;
            return parseError(ILLEGAL_SYMBOL_TYPE);
        }
        unsigned pos;
        if(!myStr2Unsigned(idx,pos)){
            errMsg="symbol index(" + idx + ")";
            return parseError(ILLEGAL_NUM);
        }
        if(type=='i'){
            errInt = pos;
            if(pos >= I){
                errMsg = "PI index";
                return parseError(NUM_TOO_BIG);
            }
            if(_gatelist[PIlist[pos]->getID()]->getname()!=""){
                errMsg="i";
                return parseError(REDEF_SYMBOLIC_NAME);
            }
            _gatelist[PIlist[pos]->getID()]->setname(tokens[1]);
        }
        if(type=='o'){
            errInt=pos;
            if(pos >= O){
                errMsg = "PO index";
                return parseError(NUM_TOO_BIG);
            }
            if(_gatelist[POlist[pos]->getID()]->getname()!=""){
                errMsg="o";
                return parseError(REDEF_SYMBOLIC_NAME);
            }
            _gatelist[POlist[pos]->getID()]->setname(tokens[1]);
        }
        lineNo++;
    }
    
    getDFSlist();
    file.close();
    return true;
}

/**********************************************************/
/*   class CirMgr member functions for circuit printing   */
/**********************************************************/
/*********************
Circuit Statistics
==================
  PI          20
  PO          12
  AIG        130
------------------
  Total      162
*********************/
void
CirMgr::connect()
{
    //Create fanin/fanout lists
    for(unsigned i=0;i<M+O+1;i++)
    {
        if(_gatelist[i]!=0){
            for(size_t j=0;j<_gatelist[i]->faninID.size();j++){
                _gatelist[i]->setfanin(_gatelist[_gatelist[i]->faninID[j]/2]);
            }
            sort(_gatelist[i]->fanoutID.begin(),_gatelist[i]->fanoutID.end());
            for(size_t k=0;k<_gatelist[i]->fanoutID.size();k++){
                _gatelist[i]->setfanout(_gatelist[(_gatelist[i]->fanoutID[k])/2]);
            }
        }
    }
    //Get floating gates
    for(unsigned i=0;i<M+O+1;i++){
        if(_gatelist[i]!=0){
            if(_gatelist[i]->getTypeStr()=="AIG"){
                    if(_gatelist[i]->faninlist.size()<2) { Floatlist1.push_back(_gatelist[i]);}
                    else if (_gatelist[i]->faninlist.size()==2){
                        if(_gatelist[i]->faninlist[0]->getTypeStr()=="UNDEF" ||
                           _gatelist[i]->faninlist[1]->getTypeStr()=="UNDEF")
                         Floatlist1.push_back(_gatelist[i]);
                    }
                }
            if(_gatelist[i]->getTypeStr()=="PO"){
                if(_gatelist[i]->faninlist.empty() ||
                   _gatelist[i]->faninlist[0]->getTypeStr()=="UNDEF")
                    Floatlist1.push_back(_gatelist[i]);
            }
        }
    }
    for(unsigned i=0;i<M+O+1;i++){
        if(_gatelist[i]!=0){
            if(_gatelist[i]->getTypeStr()=="AIG" || _gatelist[i]->getTypeStr()=="PI"){
                if(_gatelist[i]->fanoutlist.empty()) Floatlist2.push_back(_gatelist[i]);
            }
        }
    }
}
void
CirMgr::printSummary() const
{
    cout << endl;
    cout << "Circuit Statistics" << endl
    << "==================" << endl
    << "  PI   "  <<  right << setw(9) << I << endl
    << "  PO   "  <<  right << setw(9) << O << endl
    << "  AIG  "  <<  right << setw(9) << A << endl
    << "------------------" << endl
    << "  Total"  <<  right << setw(9) << I+O+A << endl;
    return;
}

void
CirMgr::printNetlist() const
{
    cout<<endl;
    for(size_t i=0;i<DFSlist.size();i++){
        cout<<"["<<i<<"] ";
        DFSlist[i]->printGate();
        cout << endl;
    }
    
}

void
CirMgr::printPIs() const
{
    cout << "PIs of the circuit: ";
    for(size_t i=0;i<PIlist.size();i++){
        cout<<PIlist[i]->getID()<<" ";
    }
    cout << endl;
}

void
CirMgr::printPOs() const
{
    cout << "POs of the circuit: ";
    for(size_t i=0;i<POlist.size();i++){
        cout<<POlist[i]->getID()<<" ";
    }
    cout << endl;
}

void
CirMgr::printFloatGates() const
{
    if(!Floatlist1.empty()){
        cout<<"Gates with floating fanin(s): ";
        for(size_t i=0;i<Floatlist1.size();i++){
            cout<<Floatlist1[i]->getID()<<" ";
        }
        cout << endl;
    }
    if(!Floatlist2.empty()){
        cout<<"Gates defined but not used  : ";
        for(unsigned i=0;i<Floatlist2.size();i++){
            cout<<Floatlist2[i]->getID()<<" ";
        }
        cout<<endl;
    }
}

void
CirMgr::DFStraversal(CirGate* gate)
{
    CirGate* cur=gate;
    for(size_t i=0;i<cur->faninlist.size();++i)
    {
        if(!cur->faninlist[i]->isGlobalRef() &&
           cur->faninlist[i]->getTypeStr() != "UNDEF" ){
            cur->setToGlobalRef();
            DFStraversal(cur->faninlist[i]);
        }
    }
    DFSlist.push_back(cur);
    cur->setToGlobalRef();
}

void
CirMgr::getDFSlist()
{
    CirGate::setGlobalRef();
    DFSlist.clear();
    for(size_t i=0;i<POlist.size();i++){
        DFStraversal(POlist[i]);
    }
}

void
CirMgr::writeAag(ostream& outfile) const
{
    outfile<<"aag "<<M<<" "<<I<<" "<<L<<" "<<O<<" "<<A<<endl;
    //I
    for(size_t i=0;i<PIlist.size();i++){outfile<<2*(PIlist[i]->getID())<<endl;}
    //O
    for(size_t i=0;i<POlist.size();i++){outfile<<POlist[i]->faninID[0]<<endl;}
    //A
    for(size_t i=0;i<DFSlist.size();i++){
        if(DFSlist[i]->getTypeStr()=="AIG"){
            outfile<<2*(DFSlist[i]->getID());
            outfile<<" "<<DFSlist[i]->faninID[0]<<" "<<DFSlist[i]->faninID[1];
            outfile<<endl;
        }
    }
    //symbol
    for(unsigned i=0;i<PIlist.size();i++){
        if(PIlist[i]->getname()!="")
        outfile<<'i'<<i<<" "<<PIlist[i]->getname()<<endl;
    }
    for(unsigned i=0;i<POlist.size();i++){
        if(POlist[i]->getname()!="")
            outfile<<'o'<<i<<" "<<POlist[i]->getname()<<endl;
    }
    //comment
    outfile<<'c'<<endl;
    outfile<<"aag output by Anne Meng"<<endl;
    outfile<<"NTUEE DSnP fall,2018";
}

void
CirMgr::reset()
{
    DFSlist.clear();
    PIlist.clear();
    POlist.clear();
    Floatlist1.clear();
    Floatlist2.clear();
    lineNo=0;
    if(_gatelist==0) return;
    for(unsigned i=0;i<M+O+1;i++)
    {
        if(_gatelist[i]!=0){
            delete _gatelist[i];
            _gatelist[i]=0;
        }
    }
    delete []_gatelist;
    _gatelist=0;
}
