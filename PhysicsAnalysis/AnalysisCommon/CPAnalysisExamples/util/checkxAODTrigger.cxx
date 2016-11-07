/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ROOTCORE

///Example standalone executable using POOL to read an xAOD
///Tests the TrigDecisionTool ... shows example usage
///You can create a skeleton application yourself with: cmt new_analysisapp MyApp
///
///Author: Will Buttinger
///

#include "AthAnalysisBaseComps/AthAnalysisHelper.h" //tool creation and configuration
#include "POOLRootAccess/TEvent.h" //event looping
#include "GaudiKernel/ToolHandle.h" //for better working with tools

#include "AsgTools/MessageCheck.h" //messaging
using namespace asg::msgUserCode;  //messaging

//EDM includes

//ROOT includes
#include "TString.h"
#include "TSystem.h"

//specific includes for this example
#include "TrigDecisionTool/TrigDecisionTool.h"

int main( int argc, char* argv[] ) {

   IAppMgrUI* app = POOL::Init(); //important to do this first! 

   // The application's name:
   //const char* APP_NAME = argv[ 0 ];

   // Open the input file:
   TString fileName = "$ASG_TEST_FILE_MC";
   std::string trigger;
   if( argc < 3 ) {
      ANA_MSG_WARNING("Usage: checkxAODTrigger.exe <trigger> <file>");
      return -1;
   }
   trigger = argv[1];
   fileName = argv[2]; //use the user provided file

   ANA_MSG_INFO("Opening file: " << gSystem->ExpandPathName(fileName.Data()) );

   //The preferred way to create and configure tools is with a ToolHandle:
   //constructor argument is: Type/Name
   ToolHandle<Trig::TrigDecisionTool> tdt("Trig::TrigDecisionTool/TrigDecisionTool");
   tdt.retrieve(); //this will cause the tool to be created and initialized

   //loop over input file with kClassAccess
   POOL::TEvent evt(POOL::TEvent::kClassAccess);
   evt.readFrom( fileName );

   int passCount(0);
   for(int i=0;i < evt.getEntries(); i++) {
      if( evt.getEntry(i) < 0) { ANA_MSG_ERROR("Failed to read event " << i); continue; }
      if(tdt->isPassed(trigger)) passCount++;
   }

   app->finalize(); //trigger finalization of all services and tools created by the Gaudi Application

   ANA_MSG_INFO("Number of events passing trigger " << trigger << ": " << passCount);

   return 0;
}

#endif
