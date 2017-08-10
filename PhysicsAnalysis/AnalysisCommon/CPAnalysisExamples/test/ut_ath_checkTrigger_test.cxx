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
#include "TStopwatch.h"
#include "TH1D.h"
#include "TFile.h"

//specific includes for this test
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "GaudiKernel/IChronoStatSvc.h"

int main( int argc, char* argv[] ) {

   IAppMgrUI* app = POOL::Init(); //important to do this first!

   // The application's name:
   const char* APP_NAME = argv[ 0 ];

   // Open the input file:
   TString fileName = "$ASG_TEST_FILE_MC";
   std::string trigger;
   if( argc < 2 ) {
      ANA_MSG_WARNING("Usage: ut_ath_checkTrigger_test.exe <trigger> <file>");
      ANA_MSG_WARNING("Using trigger = L1_2EM15");
      trigger = "L1_2EM15";
   } else {
      trigger = argv[1];
   } 
   POOL::TEvent::EReadMode mode = POOL::TEvent::kPOOLAccess; //POOL is slowest, but it can read everything!
   if( argc < 3 ) {
      ANA_MSG_WARNING( "No file name received, using $ASG_TEST_FILE_MC" );
      if (getenv ("ASG_TEST_FILE_MC") == nullptr)
        fileName = "/afs/cern.ch/user/a/asgbase/patspace/xAODs/r7725/mc15_13TeV.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.merge.AOD.e3698_s2608_s2183_r7725_r7676/AOD.07915862._000100.pool.root.1";
      
   } else {
      fileName = argv[2]; //use the user provided file
      //assume it's a DxAOD, switch to faster mode
      mode = POOL::TEvent::kClassAccess;
   }
   ANA_MSG_INFO("Opening file: " << gSystem->ExpandPathName(fileName.Data()) );

   //The preferred way to create and configure tools is with a ToolHandle:
   //constructor argument is: Type/Name
   ToolHandle<Trig::TrigDecisionTool> tdt("Trig::TrigDecisionTool/TrigDecisionTool");
   tdt.retrieve(); //this will cause the tool to be created and initialized

   //loop over input file with xAODRootAccess mode
   POOL::TEvent evt(mode);
   evt.readFrom( fileName );

   //example of accessing a service, here we will make the ChronoStatSvc display information
   ServiceHandle<IChronoStatSvc> chronoSvc("ChronoStatSvc",APP_NAME);
   AAH::setProperty( chronoSvc , "ChronoPrintOutTable", true );
   AAH::setProperty( chronoSvc , "PrintUserTime", true ); //display CPU time
   AAH::setProperty( chronoSvc , "PrintEllapsedTime", true ); //display wall clock time
   AAH::setProperty( evt.evtLoop() , "UseDetailChronoStat" , true ); //enable chrono information tracking for our event loop

   evt.getEntry(0); //first event here so dont count in timer stats
   int passCount(0);
   TStopwatch timer;
   timer.Start();
   for(int i=0;i < evt.getEntries(); i++) {
      if( evt.getEntry(i) < 0) { ANA_MSG_ERROR("Failed to read event " << i); continue; }
      if(tdt->isPassed(trigger)) passCount++;
   }
   timer.Stop();

   TFile f1("Test_ut_ath_checkTrigger_test.results.root","RECREATE");
   TH1D* h = new TH1D("processingRate","Trigger Processing Rate in Hz",1,0,1);
   h->Sumw2();
   h->SetBinContent(1,double(evt.getEntries())/timer.RealTime());
   h->SetBinError(1,10); //allow +/- 10Hz fluctuations
   h->Write();
   h = new TH1D("pass_L1_2EM15","Number of events passing L1_2EM15",1,0,1);
   h->Sumw2();
   h->SetBinContent(1,passCount);h->SetBinError(1,0.0001);
   h->Write();
   f1.Close();

   app->finalize(); //trigger finalization of all services and tools created by the Gaudi Application

   ANA_MSG_INFO("Number of events passing trigger " << trigger << ": " << passCount);

   return 0;
}

#endif
