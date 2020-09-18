/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "L1TopoConfig/L1TopoXMLParser.h"
#include "L1TopoConfig/L1TopoConfigAlg.h"

#include <iostream>
#include <string>

using namespace std;
using namespace TXC;

void traitTests(){
  static_assert(std::is_nothrow_move_constructible<L1TopoMenu>::value);
  static_assert(std::is_nothrow_move_constructible<L1TopoConfigAlg>::value);
  static_assert(std::is_nothrow_move_constructible<TXC::L1TopoConfigGlobal>::value);//TXC::L1TopoConfigOutputList
  static_assert(std::is_nothrow_move_constructible<TXC::L1TopoConfigOutputList>::value);
  static_assert(std::is_nothrow_move_constructible<TXC::TopoConfigElement>::value);
}


int main(int argc, char* argv[]) {
   
   if(argc!=2) {
      cerr << "Configuration file missing!" << endl
           << "Usage " << argv[0] << " <config.xml>" << endl;
      return 1;
   }
  
  TXC::L1TopoXMLParser L1TopoXMLParser;
  L1TopoXMLParser.readConfiguration(argv[1]);
  L1TopoXMLParser.parseConfiguration();
  L1TopoXMLParser.printConfiguration();
  
  return 0;
}
