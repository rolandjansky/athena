/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* **********************************************************************\
 *                                                                      *
 *      Name: JetCalibTools_Example                                     *
 *      Purpose: Example code for JetCalibTools	 	                *
 *                                                                      *
 *  #   Date    Comments                   By                           *
 * -- -------- -------------------------- ----------------------------- *
 *  1 14/03/16  First Version              J. Bossio (jbossios@cern.ch) * 
\************************************************************************/

// System include(s):
#include <memory>

#include "AsgTools/ToolHandle.h"
#include "AsgTools/AsgTool.h"

// ROOT
#include "TFile.h"

#ifdef XAOD_STANDALONE
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/TStore.h"
#else
#include "POOLRootAccess/TEvent.h"
#include "StoreGate/StoreGateSvc.h"
#endif

//xAOD EDM classes
#include "xAODEventInfo/EventInfo.h"
#include "xAODJet/JetContainer.h"
#include "xAODEventShape/EventShape.h"
#include "xAODRootAccess/tools/Message.h"
#include "xAODRootAccess/tools/ReturnCheck.h"
#include "xAODCore/tools/IOStats.h"
#include "xAODCore/tools/ReadStats.h"
#include "xAODCore/ShallowCopy.h"

//JetCalibrationTool
#include "JetCalibTools/JetCalibrationTool.h"

void usage() {
  std::cout << "Running options:" << std::endl;
  std::cout << "	--help : To get the help you're reading" << std::endl;
  std::cout << "	--jetCalibConfig= : Specify the JetCalibTools config" << std::endl;
  std::cout << "	--jetColl= : Specify the jet collection" << std::endl;
  std::cout << "	--calibSeq= : Specify the calibration sequence for JetCalibTools" << std::endl;
  std::cout << "	--isData=TRUE : Specify isData true for JetCalibTools" << std::endl;
  std::cout << "	--isData=FALSE : Specify isData false for JetCalibTools" << std::endl;
  std::cout << "	--sample= : Specify input xAOD" << std::endl;
  std::cout << "        Example: Example --jetCalibConfig=JES_2015dataset_recommendation_Feb2016.config --jetColl=AntiKt4EMTopo --calibSeq=JetArea_Residual_Origin_EtaJES_GSC --isData=FALSE --sample=xAOD.root" << std::endl;
}

//---------------
// Main Function
//---------------

int main(int argc, char* argv[]){

  //---------------------------------------------
  // Declaring input variables with default values
  //---------------------------------------------
  std::string isData = "";
  std::string sample = "";
  std::string jetColl = "";
  std::string jetCalibConfig = "";
  std::string calibSeq = "";
  bool isCollision = false;

  //---------------------------
  // Decoding the user settings
  //---------------------------
  for (int i=1; i< argc; i++){

    std::string opt(argv[i]); std::vector< std::string > v;

    std::istringstream iss(opt);

    std::string item;
    char delim = '=';

    while (std::getline(iss, item, delim)){
        v.push_back(item);
    }

    if ( opt.find("--help") != std::string::npos ) {
      usage(); return 0;
    }

    if ( opt.find("--sample=")   != std::string::npos ) sample = v[1];
    
    if ( opt.find("--jetColl=")   != std::string::npos ) jetColl = v[1];
    
    if ( opt.find("--jetCalibConfig=")   != std::string::npos ) jetCalibConfig = v[1];
    
    if ( opt.find("--calibSeq=")   != std::string::npos ) calibSeq = v[1];

    if ( opt.find("--isData=")   != std::string::npos ) isData = v[1];

  }//End: Loop over input options

  if(sample==""){
    std::cout << "No input xAOD file specified, exiting" << std::endl;
    return 1;
  }
  if(jetColl==""){
    std::cout << "No jet collection specified, exiting" << std::endl;
    return 1;
  }
  if(jetCalibConfig==""){
    std::cout << "No JetCalibTools config specified, exiting" << std::endl;
    return 1;
  }
  if(calibSeq==""){
    std::cout << "No calibration sequence specified, exiting" << std::endl;
    return 1;
  }
  if(isData==""){
    std::cout << "isData not specified, exiting" << std::endl;
    return 1;
  }
  else if(isData=="TRUE") isCollision = true;

  // Set up the job for xAOD access:
  static const char* APP_NAME = "JetCalibTools_Example";
 
  //--------------------
  // Opening input file
  //--------------------
  std::unique_ptr< TFile > ifile( TFile::Open( sample.c_str(), "READ" ) );

  // Create a TEvent object.
#ifdef XAOD_STANDALONE
  RETURN_CHECK( APP_NAME, xAOD::Init() );
  xAOD::TEvent event( xAOD::TEvent::kClassAccess );
  RETURN_CHECK( APP_NAME, event.readFrom( ifile.get() ) );
#else // Athena "Store" is the same StoreGate used by the TEvent
  POOL::TEvent event( POOL::TEvent::kClassAccess );
  CHECK_WITH_CONTEXT( event.readFrom( ifile.get() ), APP_NAME, 1 );
#endif
 

  //----------------------------------
  // Initialization of JetCalibTools
  //----------------------------------
  const std::string name_JetCalibTools = "JetCalib_Example";

  // Call the constructor
  JetCalibrationTool jetCalibrationTool(name_JetCalibTools.c_str());
  CHECK_WITH_CONTEXT( jetCalibrationTool.setProperty("JetCollection",jetColl.c_str()),
                      APP_NAME, 1 );

  CHECK_WITH_CONTEXT( jetCalibrationTool.setProperty("CalibSequence",calibSeq.c_str()),
                      APP_NAME, 1 );

  CHECK_WITH_CONTEXT( jetCalibrationTool.setProperty("ConfigFile",jetCalibConfig.c_str()),
                      APP_NAME, 1 );

  CHECK_WITH_CONTEXT( jetCalibrationTool.setProperty("IsData",isCollision),
                      APP_NAME, 1 );

  // Initialize the tool
  if(!(jetCalibrationTool.initialize().isSuccess())){
    std::cout << "Initialization of JetCalibTools failed, exiting" << std::endl;
    return 0;
  }

  //------------------
  // Loop over events
  //------------------

  const Long64_t nevents = event.getEntries();
  for(Long64_t ievent = 0;  ievent < nevents; ++ievent){

     // Load the event:
     if( event.getEntry( ievent ) < 0 ) {
        std::cerr << "Failed to load entry " << ievent << std::endl;
        return 1;
     }

    // Show status
    if(ievent % 100==0) std::cout << "Event " << ievent << " of " << nevents << std::endl;

    // Retrieve jet container
    const xAOD::JetContainer* jets = 0;
    CHECK_WITH_CONTEXT( event.retrieve( jets, jetColl + "Jets" ), APP_NAME, 1 );

    // Shallow copy 
    auto jets_shallowCopy = xAOD::shallowCopyContainer( *jets );

    // Iterate over the shallow copy
    for( xAOD::Jet* jet : *( jets_shallowCopy.first ) ) {
       CHECK_WITH_CONTEXT( jetCalibrationTool.applyCalibration( *jet ), APP_NAME, 1 );
      // Do something
    }
    delete jets_shallowCopy.first;
    delete jets_shallowCopy.second;
  
  }//END: Loop over events

  xAOD::IOStats::instance().stats().printSmartSlimmingBranchList();

  return 0;
}
