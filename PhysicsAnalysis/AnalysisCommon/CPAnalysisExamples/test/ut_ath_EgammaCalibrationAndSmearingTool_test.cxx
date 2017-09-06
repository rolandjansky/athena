/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ROOTCORE

///Example standalone executable using POOL to read an xAOD
///Tests the EgammaCalibrationAndSmearingTool
///You can create a skeleton application yourself with: cmt new_analysisapp MyApp
///
///See src/Test__EgammaCalibrationAndSmearingTool.cxx and share/Test_EgammaCalibrationAndSmearingToolJobOptions.py 
///for how to do the equivalent as an athena framework job
///
///Author: Will Buttinger
///

#include "AthAnalysisBaseComps/AthAnalysisHelper.h" //tool creation and configuration
#include "POOLRootAccess/TEvent.h" //event looping
#include "GaudiKernel/ToolHandle.h" //for better working with tools

#include "AsgTools/MessageCheck.h" //messaging
using namespace asg::msgUserCode;  //messaging

//EDM includes
#include "xAODEgamma/ElectronContainer.h"
#include "xAODCore/ShallowCopy.h"

//ROOT includes
#include "TString.h"
#include "TSystem.h"

//specific includes for this test
#include "ElectronPhotonFourMomentumCorrection/IEgammaCalibrationAndSmearingTool.h"
#include "GaudiKernel/IChronoStatSvc.h"

int main( int argc, char* argv[] ) {

   IAppMgrUI* app = POOL::Init(); //important to do this first!

   // The application's name:
   const char* APP_NAME = argv[ 0 ];

   // Open the input file:
   TString fileName = "$ASG_TEST_FILE_MC";
   if( argc < 2 ) {
      ANA_MSG_WARNING( "No file name received, using $ASG_TEST_FILE_MC" );
      if (getenv ("ASG_TEST_FILE_MC") == nullptr)
        fileName = "/afs/cern.ch/user/a/asgbase/patspace/xAODs/r7725/mc15_13TeV.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.merge.AOD.e3698_s2608_s2183_r7725_r7676/AOD.07915862._000100.pool.root.1";
   } else {
      fileName = argv[1]; //use the user provided file
   }
   ANA_MSG_INFO("Opening file: " << gSystem->ExpandPathName(fileName.Data()) );

   //create instance of EgammaCalibrationAndSmearingTool 
   //will request with the interface class: CP::IEgammaCalibrationAndSmearingTool
   auto myTool = AthAnalysisHelper::createTool<CP::IEgammaCalibrationAndSmearingTool>("CP::EgammaCalibrationAndSmearingTool/myTool");
   AthAnalysisHelper::setProperty( myTool, "ESModel", "es2015PRE" );
   myTool->initialize();

   //The preferred way to create and configure the tool is with a ToolHandle:
   ToolHandle<CP::IEgammaCalibrationAndSmearingTool> myBetterTool("CP::EgammaCalibrationAndSmearingTool/myTool");
   AthAnalysisHelper::setProperty( myBetterTool, "ESModel", "es2015PRE" );
   myBetterTool.retrieve(); //this will cause the tool to be created and initialized

   //loop over input file with POOL 
   POOL::TEvent evt;
   evt.readFrom( fileName );

   //example of accessing a service, here we will make the ChronoStatSvc display information
   ServiceHandle<IChronoStatSvc> chronoSvc("ChronoStatSvc",APP_NAME);
   AthAnalysisHelper::setProperty( chronoSvc , "PrintUserTime", true ); //display CPU time
   AthAnalysisHelper::setProperty( chronoSvc , "PrintEllapsedTime", true ); //display wall clock time

   AthAnalysisHelper::setProperty( evt.evtLoop() , "UseDetailChronoStat" , true ); //enable chrono information tracking for our event loop

   int nEntries = 500; //evt.getEntries();

   for(int i=0;i < nEntries; i++) {
      if( evt.getEntry(i) < 0) { ANA_MSG_ERROR("Failed to read event " << i); continue; }

      //get the electrons
      const xAOD::ElectronContainer* xaod_els = 0;
      if( evt.retrieve( xaod_els , "Electrons" ).isFailure() ) {
         ANA_MSG_ERROR("Could not retrieve electrons"); return -1;
      }

      //create a shallow copy and calibrate 
      auto calibratedElectrons = xAOD::shallowCopyContainer( *xaod_els );
      for(auto el : *calibratedElectrons.first) myTool->applyCorrection(*el).setChecked();

      //for first 10 events we will print out the calibrated and uncalibrated Pt for comparison
      if(i<10) {
         for(uint j=0;j<xaod_els->size();j++) {
            ANA_MSG_INFO("Uncalibrated Pt = " << xaod_els->at(j)->pt() << " , Calibrated Pt = " << calibratedElectrons.first->at(j)->pt());
         }
      }

      //cleanup the shallow copy 
      delete calibratedElectrons.first;
      delete calibratedElectrons.second; //the auxstore 

   }


   delete myTool;

   app->finalize(); //trigger finalization of all services and tools created by the Gaudi Application

   return 0;
}

#endif
