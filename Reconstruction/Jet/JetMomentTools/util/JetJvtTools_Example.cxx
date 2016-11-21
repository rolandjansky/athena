/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* **********************************************************************\
 *                                                                      *
 *      Name: JetJvtTools_Example                                     *
 *      Purpose: Example code for JetJvtTools	 	                *
 *                                                                      *
 *  #   Date    Comments                   By                           *
 * -- -------- -------------------------- ----------------------------- *
 *  1 14/03/16  First Version              J. Bossio (jbossios@cern.ch) * 
\************************************************************************/

// This will only run in RootCore
#ifdef ROOTCORE

// System include(s):
#include <memory>

#include "AsgTools/ToolHandle.h"
#include "AsgTools/AsgTool.h"

// ROOT
#include "TFile.h"

//xAOD EDM classes
#include "xAODEventInfo/EventInfo.h"
#include "xAODJet/JetContainer.h"
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/TStore.h"
#include "xAODRootAccess/tools/Message.h"
#include "xAODRootAccess/tools/ReturnCheck.h"
#include "xAODCore/tools/IOStats.h"
#include "xAODCore/tools/ReadStats.h"
#include "xAODCore/ShallowCopy.h"

//JetCalibrationTool
#include "JetMomentTools/JetVertexTaggerTool.h"
#include "JetMomentTools/JetForwardJvtTool.h"

void usage() {
  std::cout << "Running options:" << std::endl;
  std::cout << "	--help : To get the help you're reading" << std::endl;
  std::cout << "	--jetColl= : Specify the jet collection" << std::endl;
  std::cout << "	--sample= : Specify input xAOD" << std::endl;
  std::cout << "        Example: Example --jetColl=AntiKt4EMTopo --sample=xAOD.root" << std::endl;
}

//---------------
// Main Function
//---------------

int main(int argc, char* argv[]){

  //---------------------------------------------
  // Declaring input variables with default values
  //---------------------------------------------
  std::string sample = "";
  std::string jetColl = "";

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
    
  }//End: Loop over input options

  if(sample==""){
    std::cout << "No input xAOD file specified, exiting" << std::endl;
    return 1;
  }
  if(jetColl==""){
    std::cout << "No jet collection specified, exiting" << std::endl;
    return 1;
  }

  // Set up the job for xAOD access:
  static const char* APP_NAME = "JetJvtTools_Example";
  RETURN_CHECK( APP_NAME, xAOD::Init() );
 
  //--------------------
  // Opening input file
  //--------------------
  std::unique_ptr< TFile > ifile( TFile::Open( sample.c_str(), "READ" ) );

  // Create a TEvent object.
  xAOD::TEvent event( xAOD::TEvent::kClassAccess );
  RETURN_CHECK( APP_NAME, event.readFrom( ifile.get() ) );

  // Create a transient object store. Needed for the tools.
  xAOD::TStore store;

  //----------------------------------
  // Initialization of JetJvtTools
  //----------------------------------
  const std::string name_JetJvtTool = "JetJvt_Example";
  const std::string name_JetfJvtTool = "JetfJvt_Example";

  // Call the constructor
  JetVertexTaggerTool jvtTool(name_JetJvtTool.c_str());
  JetForwardJvtTool fjvtTool(name_JetfJvtTool.c_str());
  RETURN_CHECK(APP_NAME,
               jvtTool.setProperty("JVTFileName",
                                              "JetMomentTools/JVTlikelihood_20140805.root"));

  // Initialize the tool
  if(!(jvtTool.initialize().isSuccess())){
    std::cout << "Initialization of JetJvtTools failed, exiting" << std::endl;
    return 0;
  }
  if(!(fjvtTool.initialize().isSuccess())){
    std::cout << "Initialization of JetJvtTools failed, exiting" << std::endl;
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
    RETURN_CHECK( APP_NAME, event.retrieve( jets, jetColl + "Jets" ) );

    // Shallow copy 
    auto jets_shallowCopy = xAOD::shallowCopyContainer( *jets );

    // Iterate over the shallow copy
    for( xAOD::Jet* jet : *( jets_shallowCopy.first ) ) {
      jvtTool.updateJvt( *jet );
      // Do something
    }
    fjvtTool.modify(*( jets_shallowCopy.first ));
    delete jets_shallowCopy.first;
    delete jets_shallowCopy.second;
  
  }//END: Loop over events

  xAOD::IOStats::instance().stats().printSmartSlimmingBranchList();

  return 0;
}

#endif
