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

#include <AsgTools/MessageCheck.h>

#include <TSystem.h>
#include <iostream>
#include <string>

int main( int /*argc*/, char** /*argv[]*/ ) {

  SH::SampleHandler sh;
  using namespace asg::msgUserCode;

  try {
    readSusyMetaDir(sh, "$ROOTCOREBIN/data/SUSYTools/mc15_13TeV");
    readSusyMetaDir(sh, "$ROOTCOREBIN/data/SUSYTools/mc15_8TeV");
  }
  catch (std::exception e) {
    ATH_MSG_ERROR ("A problem was encountered when reading the xsec files: "<< e.what());
    ATH_MSG_ERROR ("Will exit and return 1 to fail unit test check");
    return 1;
  }
  ATH_MSG_INFO("ALL GOOD!");
  return 0;
}


