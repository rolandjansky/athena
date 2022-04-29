/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfMuctpi/MuctpiXMLParser.h"
#include "TrigConfMuctpi/MuCTPiGeometry.h"

#include <iostream>
#include <string>

using namespace std;

int main(int argc, char* argv[]) {
   
   if(argc!=2) {
      cerr << "Configuration file missing!" << endl
           << "Usage " << argv[0] << " <config.xml>" << endl;
      return 1;
   }
  
  MuctpiXMLParser XMLParser;
  XMLParser.readConfiguration(argv[1]);
  XMLParser.parseConfiguration();
  //XMLParser.printConfiguration();
  
  std::filebuf fb;
  if (fb.open ("output.xml",std::ios::out) == nullptr) {
    return 1;
  }
  std::ostream os(&fb);
  XMLParser.getMuCTPiGeometry().writeXML(os);
  fb.close();
  return 0;
}
