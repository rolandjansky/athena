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
#include "AsgTools/AnaToolHandle.h"
#include "AsgTools/MessageCheck.h"
#include "xAODCore/tools/IOStats.h"
#include "xAODCore/tools/ReadStats.h"
#include "xAODCore/ShallowCopy.h"

//JetCalibrationTool
#include "JetInterface/IJetUpdateJvt.h"
#include "JetInterface/IJetModifier.h"

void usage() {
  std::cout << "Running options:" << std::endl;
  std::cout << "	--help : To get the help you're reading" << std::endl;
  std::cout << "	--jetColl= : Specify the jet collection" << std::endl;
  std::cout << "	--sample= : Specify input xAOD" << std::endl;
  std::cout << "	--nevents= : Specify maximum number of events to run" << std::endl;
  std::cout << "        Example: Example --jetColl=AntiKt4EMTopo --sample=xAOD.root" << std::endl;
}

//---------------
// Main Function
//---------------

int main(int argc, char* argv[]){
  using namespace asg::msgUserCode;
  ANA_CHECK_SET_TYPE (int)

  //---------------------------------------------
  // Declaring input variables with default values
  //---------------------------------------------
  std::string sample = "";
  std::string jetColl = "";
  Long64_t nevents = -1;

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

    if ( opt.find("--nevents=")   != std::string::npos ) nevents = atoi(v[1].c_str());
    
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
 
  //--------------------
  // Opening input file
  //--------------------
  std::unique_ptr< TFile > ifile( TFile::Open( sample.c_str(), "READ" ) );
  // Create a TEvent object.
  ANA_CHECK( xAOD::Init() );
  xAOD::TEvent event( xAOD::TEvent::kClassAccess );
  ANA_CHECK( event.readFrom( ifile.get() ) );

  //----------------------------------
  // Initialization of JetJvtTools
  //----------------------------------

  // Create the tools
  asg::AnaToolHandle<IJetUpdateJvt> jvtTool("JetVertexTaggerTool/JetJvt_Example");
  asg::AnaToolHandle<IJetModifier> fjvtTool("JetForwardJvtTool/JetfJvt_Example");
  
  // Initialize the tools
  if(!(jvtTool.retrieve().isSuccess())){
    std::cout << "Initialization of JetVertexTaggerTool failed, exiting" << std::endl;
    return 0;
  }
  if(!(fjvtTool.retrieve().isSuccess())){
    std::cout << "Initialization of JetForwardJvtTool failed, exiting" << std::endl;
    return 0;
  }

  //------------------
  // Loop over events
  //------------------

  if(nevents<0) {nevents = event.getEntries();}
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
    ANA_CHECK( event.retrieve( jets, jetColl + "Jets" ) );
    std::cout << "Retrieved jets" << std::endl;

    // Shallow copy 
    auto jets_shallowCopy = xAOD::shallowCopyContainer( *jets );
    std::cout << "Copied jets" << std::endl;

    // Iterate over the shallow copy
    for( xAOD::Jet* jet : *( jets_shallowCopy.first ) ) {
      std::cout << "On jet " << jet->index() << std::endl;
      // Here, we just apply a dummy calibration.
      // You should use JetCalibTools to apply the recommended calibration
      jet->setJetP4( xAOD::JetFourMom_t(jet->pt()*1.6, jet->eta(), jet->phi(), jet->m()) );
      std::cout << "  uncorrected JVT: " << jet->getAttribute<float>("Jvt") << std::endl;
      std::cout << "  corrected JVT: " << jvtTool->updateJvt( *jet ) << std::endl;
      // Do something
    }
    fjvtTool->modify(*( jets_shallowCopy.first ));
    delete jets_shallowCopy.first;
    delete jets_shallowCopy.second;
  }//END: Loop over events

  xAOD::IOStats::instance().stats().printSmartSlimmingBranchList();

  return 0;
}
