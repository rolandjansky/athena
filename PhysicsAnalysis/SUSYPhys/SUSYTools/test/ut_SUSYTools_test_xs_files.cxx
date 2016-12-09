/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//---------------------------------------------------------------
//  Test cross section files
//---------------------------------------------------------------

#include "SampleHandler/SampleHandler.h"
#include "SampleHandler/ToolsDiscovery.h"
#include "SampleHandler/ToolsMeta.h"
#include "SampleHandler/ScanDir.h"

#include <TSystem.h>
#include <iostream>
#include <string>

int main( int /*argc*/, char** /*argv[]*/ ) {

  SH::SampleHandler sh;
  
  try {
    readSusyMetaDir(sh, "$ROOTCOREBIN/data/SUSYTools/mc15_13TeV");
    readSusyMetaDir(sh, "$ROOTCOREBIN/data/SUSYTools/mc15_8TeV");
  }
  catch (std::exception e) {
    std::cout << "A problem was encountered when reading the xsec files: "
	      << e.what() << std::endl;
    std::cout << "Will exit and return 1 to fail unit test check" << std::endl;
    return 1;
  }
  std::cout << "ALL GOOD!" << std::endl;
  return 0;
}


