/****************************************************************************
  FileName     [ p2Main.cpp ]
  PackageName  [ p2 ]
  Synopsis     [ Define main() function ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2016-present DVLab, GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <string>
#include <iomanip>
#include "p2Json.h"

using namespace std;

int main()
{
   Json json;

   // Read in the csv file. Do NOT change this part of code.
   string jsonFile;
   cout << "Please enter the file name: ";
   cin >> jsonFile;
   if (json.read(jsonFile))
      cout << "File \"" << jsonFile << "\" was read in successfully." << endl;
   else {
      cerr << "Failed to read in file \"" << jsonFile << "\"!" << endl;
      exit(-1); // jsonFile does not exist.
   }
    string k;
    int v;
   while (true) {
       cout << "Enter command: ";
       string command;
       cin>>command;
       if(command=="PRINT") json.print();
       else if (command=="SUM") json.sum();
       else if (command=="AVE") json.ave();
       else if (command=="MIN") json.min();
       else if (command=="MAX") json.max();
       else if (command=="ADD") {
           cin>>k>>v;
           JsonElem j_add(k,v);
           json.add(j_add);
       }
       else if (command=="EXIT") return 0;
       else cout<<"ERROR COMMAND"<<endl;
   }
}
