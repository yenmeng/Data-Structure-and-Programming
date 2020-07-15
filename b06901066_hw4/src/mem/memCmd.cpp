/****************************************************************************
  FileName     [ memCmd.cpp ]
  PackageName  [ mem ]
  Synopsis     [ Define memory test commands ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <iomanip>
#include <sstream>
#include "memCmd.h"
#include "memTest.h"
#include "cmdParser.h"
#include "util.h"

using namespace std;

extern MemTest mtest;  // defined in memTest.cpp

bool
initMemCmd()
{
   if (!(cmdMgr->regCmd("MTReset", 3, new MTResetCmd) &&
         cmdMgr->regCmd("MTNew", 3, new MTNewCmd) &&
         cmdMgr->regCmd("MTDelete", 3, new MTDeleteCmd) &&
         cmdMgr->regCmd("MTPrint", 3, new MTPrintCmd)
      )) {
      cerr << "Registering \"mem\" commands fails... exiting" << endl;
      return false;
   }
   return true;
}


//----------------------------------------------------------------------
//    MTReset [(size_t blockSize)]
//----------------------------------------------------------------------
CmdExecStatus
MTResetCmd::exec(const string& option)
{
   // check option
   string token;
   if (!CmdExec::lexSingleOption(option, token))
      return CMD_EXEC_ERROR;
   if (token.size()) {
      int b;
      if (!myStr2Int(token, b) || b < int(toSizeT(sizeof(MemTestObj)))) {
         cerr << "Illegal block size (" << token << ")!!" << endl;
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, token);
      }
      #ifdef MEM_MGR_H
      mtest.reset(toSizeT(b));
      #else
      mtest.reset();
      #endif // MEM_MGR_H
   }
   else
      mtest.reset();
   return CMD_EXEC_DONE;
}

void
MTResetCmd::usage(ostream& os) const
{  
   os << "Usage: MTReset [(size_t blockSize)]" << endl;
}

void
MTResetCmd::help() const
{  
   cout << setw(15) << left << "MTReset: " 
        << "(memory test) reset memory manager" << endl;
}


//----------------------------------------------------------------------
//    MTNew <(size_t numObjects)> [-Array (size_t arraySize)]
//----------------------------------------------------------------------
CmdExecStatus
MTNewCmd::exec(const string& option)
{
   // TODO
    vector<string> options;
    if (!CmdExec::lexOptions(option, options))
        return CMD_EXEC_ERROR;
    if (options.empty())
        return CmdExec::errorOption(CMD_OPT_MISSING, "");
    size_t numobjects=0;
    size_t arrsize=0;
    int num;
    int arr;
    bool doArray = false;
    bool size = false;
    bool a_size = false;
    for(size_t i=0;i<options.size();i++){
        if(i>2) return CmdExec::errorOption(CMD_OPT_EXTRA,options[i]);
        if(myStrNCmp("-Array", options[i], 2) == 0){
            if(doArray) return CmdExec::errorOption(CMD_OPT_EXTRA,options[i]);
            doArray=true;
        }
        int n=0;
        if(!myStr2Int(options[i],n) && myStrNCmp("-Array", options[i], 2) != 0)
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
        if(myStr2Int(options[i],n)) {
            if(n<=0) return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
            if(!doArray){
                if(size) return CmdExec::errorOption(CMD_OPT_EXTRA, options[i]);
                else size=true;
                numobjects = (size_t)n;
            }
            else if(doArray){
                if((!a_size && !size) || size){
                    arrsize = (size_t)n;
                    a_size=true;
                }
                else numobjects = (size_t)n;
            }
        }
    }
    if(doArray && options.size()!=3)
        return CmdExec::errorOption(CMD_OPT_MISSING, "");
    try{
        if(doArray) mtest.newArrs( numobjects , arrsize );
        else        mtest.newObjs( numobjects );
    }
    catch(bad_alloc &e){
        return CMD_EXEC_ERROR;
    }
    
}

void
MTNewCmd::usage(ostream& os) const
{  
   os << "Usage: MTNew <(size_t numObjects)> [-Array (size_t arraySize)]\n";
}

void
MTNewCmd::help() const
{  
   cout << setw(15) << left << "MTNew: " 
        << "(memory test) new objects" << endl;
}


//----------------------------------------------------------------------
//    MTDelete <-Index (size_t objId) | -Random (size_t numRandId)> [-Array]
//----------------------------------------------------------------------
CmdExecStatus
MTDeleteCmd::exec(const string& option)
{
   // TODO
    vector<string> options;
    if (!CmdExec::lexOptions(option, options))
        return CMD_EXEC_ERROR;
    if (options.empty())
        return CmdExec::errorOption(CMD_OPT_MISSING, "");
    bool doIndex = false;
    bool doRandom = false;
    bool doArray = false;
    bool I = false;
    size_t R_obj = mtest.getObjListSize();
    size_t R_arr = mtest.getArrListSize();
    size_t _idx=0;
    size_t rand=0;
    size_t temp=0;
    size_t arrpos=0;
    size_t randpos=0;
    for(size_t i=0;i<options.size();i++){
        if(i>3) return CmdExec::errorOption(CMD_OPT_EXTRA,options[i]);
        if(myStrNCmp("-Array", options[i], 2) == 0){
            if(doArray) return CmdExec::errorOption(CMD_OPT_EXTRA,options[i]);
            doArray=true;
            arrpos=i;
        }
        if (myStrNCmp("-Index", options[i], 2) == 0) {
            if (doIndex || doRandom){
                if(!I) return CmdExec::errorOption(CMD_OPT_ILLEGAL,options[i]);
                else return CmdExec::errorOption(CMD_OPT_EXTRA, options[i]);
            }
            doIndex=true;
        }
        if (myStrNCmp("-Random", options[i], 2) == 0) {
            if (doIndex || doRandom){
                if(!I) return CmdExec::errorOption(CMD_OPT_ILLEGAL,options[i]);
                else return CmdExec::errorOption(CMD_OPT_EXTRA, options[i]);
            }
            doRandom=true;
            randpos=i;
        }
        int n=0;
        if(!myStr2Int(options[i],n) && myStrNCmp("-Random", options[i], 2) != 0 && myStrNCmp("-Index", options[i], 2) != 0 && myStrNCmp("-Array", options[i], 2) != 0){
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
        }
        if(myStr2Int(options[i],n)){
            if(n<0) return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
            if(i==0 || (!doIndex && !doRandom)) return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
            if(I) return CmdExec::errorOption(CMD_OPT_EXTRA, options[i]);
            I=true;
            if(doIndex) _idx=(size_t)n;
            if(doRandom) rand=(size_t)n;
        }
        
    }
    if(options.size()==1) return CmdExec::errorOption(CMD_OPT_MISSING,"");
    if(options.size()==2){
        if(!doArray){
            if(doRandom){
                if(mtest.getObjListSize()==0){
                    cerr<<"Size of object list is 0!!"<<endl;
                    return CmdExec::errorOption(CMD_OPT_ILLEGAL,options[randpos]);
                }
                for(size_t i=0;i<rand;i++)
                {
                    mtest.deleteObj((size_t)rnGen(R_obj));
                }
                
            }
            else{
                if(_idx >= mtest.getObjListSize()){
                    stringstream ss;
                    ss << (int)_idx;
                    string str = ss.str();
                    cerr<<"Size of object list ("<<mtest.getObjListSize()<<")"<<" is <= "<<_idx<<"!!"<<endl;
                    return CmdExec::errorOption(CMD_OPT_ILLEGAL,str);
                }
                mtest.deleteObj(_idx);
                
            }
        }
        else {
            if(arrpos==0 && (doIndex || doRandom))
               return CmdExec::errorOption(CMD_OPT_MISSING, options[1]);
            else return CmdExec::errorOption(CMD_OPT_ILLEGAL,options[0]);
        }
        
    }
    else if(doArray){
        if(arrpos==1) return CmdExec::errorOption(CMD_OPT_ILLEGAL,options[1]);
        if(doRandom){
            if(mtest.getArrListSize()==0){
                cerr<<"Size of array list is 0!!"<<endl;
                return CmdExec::errorOption(CMD_OPT_ILLEGAL,options[randpos]);
            }
            for(size_t i=0;i<rand;i++)
            {
                mtest.deleteArr((size_t)rnGen(R_arr));
            }
            
        }
        else{
            if(_idx >= mtest.getArrListSize()){
                stringstream ss;
                ss << (int)_idx;
                string str = ss.str();
                cerr<<"Size of array list ("<<mtest.getArrListSize()<<")"<<" is <= "<<_idx<<"!!"<<endl;
                return CmdExec::errorOption(CMD_OPT_ILLEGAL,str);
            }
            mtest.deleteArr(_idx);
        }
    }
}

void
MTDeleteCmd::usage(ostream& os) const
{  
   os << "Usage: MTDelete <-Index (size_t objId) | "
      << "-Random (size_t numRandId)> [-Array]" << endl;
}

void
MTDeleteCmd::help() const
{  
   cout << setw(15) << left << "MTDelete: " 
        << "(memory test) delete objects" << endl;
}


//----------------------------------------------------------------------
//    MTPrint
//----------------------------------------------------------------------
CmdExecStatus
MTPrintCmd::exec(const string& option)
{
   // check option
   if (option.size())
      return CmdExec::errorOption(CMD_OPT_EXTRA, option);
   mtest.print();

   return CMD_EXEC_DONE;
}

void
MTPrintCmd::usage(ostream& os) const
{  
   os << "Usage: MTPrint" << endl;
}

void
MTPrintCmd::help() const
{  
   cout << setw(15) << left << "MTPrint: " 
        << "(memory test) print memory manager info" << endl;
}


