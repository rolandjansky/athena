/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "L1TopoConfig/L1TopoXMLParser.h"
#include "L1TopoConfig/L1TopoConfigAlg.h"

#include <iostream>
#include <string>

using namespace std;

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
