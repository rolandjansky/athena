/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TError.h"
#include "xAODRootAccess/Init.h"
#include "AsgTools/AnaToolHandle.h"

#include "JetResolution/IJERTool.h"
#include "JetResolution/IJERSmearingTool.h"

static const char* appName = "ut_AnaToolHandle";

#define CHECK( ARG )                                 \
  do {                                               \
    const bool result = ARG;                         \
    if(!result) {                                    \
      ::Error(appName, "Failed to execute: \"%s\"",  \
              #ARG );                                \
      return 1;                                      \
    }                                                \
  } while( false )

int main()
{
  CHECK( xAOD::Init(appName) );

  // Try to create and initialize each tool with the AnaToolHandle
  asg::AnaToolHandle<IJERTool> jerHandle("JERTool");
  CHECK( jerHandle.initialize() );
  asg::AnaToolHandle<IJERSmearingTool> smearHandle("JERSmearingTool");
  CHECK( smearHandle.initialize() );

  return 0;
}
