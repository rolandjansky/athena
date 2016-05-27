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

// This will only run in RootCore
#ifdef ROOTCORE
#include "AsgTools/ToolHandle.h"
#include "AsgTools/AsgTool.h"

// ROOT
#include "TFile.h"

//xAOD EDM classes
#include "xAODEventInfo/EventInfo.h"
#include "xAODJet/JetContainer.h"
#include "xAODEventShape/EventShape.h"
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/TStore.h"
#include "xAODRootAccess/tools/Message.h"
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
    return 0;
  }
  if(jetColl==""){
    std::cout << "No jet collection specified, exiting" << std::endl;
    return 0;
  }
  if(jetCalibConfig==""){
    std::cout << "No JetCalibTools config specified, exiting" << std::endl;
    return 0;
  }
  if(calibSeq==""){
    std::cout << "No calibration sequence specified, exiting" << std::endl;
    return 0;
  }
  if(isData==""){
    std::cout << "isData not specified, exiting" << std::endl;
    return 0;
  }
  else if(isData=="TRUE") isCollision = true;

  // Set up the job for xAOD access:
  xAOD::Init().ignore();
 
  //--------------------
  // Opening input file
  //--------------------
  std::auto_ptr< TFile > ifile( TFile::Open( sample.c_str(), "READ" ) );

  // Create a TEvent object.
  xAOD::TEvent * event = new xAOD::TEvent( xAOD::TEvent::kClassAccess );
  event->readFrom( ifile.get(), xAOD::TEvent::kClassAccess  );

  // Create a transient object store. Needed for the tools.
  xAOD::TStore store ;
  
  //----------------------------------
  // Initialization of JetCalibTools
  //----------------------------------
  const std::string name_JetCalibTools = "JetCalib_Example";

  // Call the constructor
  JetCalibrationTool *m_JetCalibrationTool = new JetCalibrationTool(name_JetCalibTools.c_str(), jetColl.c_str(), jetCalibConfig.c_str(), calibSeq.c_str(), isCollision);

  // Initialize the tool
  if(!(m_JetCalibrationTool->initializeTool(name_JetCalibTools).isSuccess())){
    std::cout << "Initialization of JetCalibTools failed, exiting" << std::endl;
    return 0;
  }

  //------------------
  // Loop over events
  //------------------

  Long64_t nevents = event->getEntries();
  for(Long64_t ievent = 0;  ievent < nevents; ++ievent){
 
    // Show status
    if(ievent % 100==0) std::cout << "Event " << ievent << " of " << nevents << std::endl;

    // Retrieve jet container
    const xAOD::JetContainer* jets = 0;
    if(jetColl == "AntiKt4EMTopo"){
      if( !(event->retrieve( jets, "AntiKt4EMTopoJets" ).isSuccess()) ){
        std::cout << "Failed to retrieve AntiKt4EMTopoJets container. Exiting." << std::endl;
        return 0;
      }
    }
    else if(jetColl == "AntiKt4EMPFlow"){
      if( !(event->retrieve( jets, "AntiKt4EMPFlowJets" ).isSuccess()) ){
        std::cout << "Failed to retrieve AntiKt4EMPFlowJets container. Exiting." << std::endl;
        return 0;
      }
    }
    else if(jetColl == "AntiKt4LCTopo"){
      if( !(event->retrieve( jets, "AntiKt4LCTopoJets" ).isSuccess()) ){
        std::cout << "Failed to retrieve AntiKt4LCTopoJets container. Exiting." << std::endl;
        return 0;
      }
    }
    else if(jetColl == "AntiKt2LCTopo"){
      if( !(event->retrieve( jets, "AntiKt2LCTopoJets" ).isSuccess()) ){
        std::cout << "Failed to retrieve AntiKt2LCTopoJets container. Exiting." << std::endl;
        return 0;
      }
    }
    else if(jetColl == "AntiKt3LCTopo"){
      if( !(event->retrieve( jets, "AntiKt3LCTopoJets" ).isSuccess()) ){
        std::cout << "Failed to retrieve AntiKt3LCTopoJets container. Exiting." << std::endl;
        return 0;
      }
    }
    else if(jetColl == "AntiKt5LCTopo"){
      if( !(event->retrieve( jets, "AntiKt5LCTopoJets" ).isSuccess()) ){
        std::cout << "Failed to retrieve AntiKt5LCTopoJets container. Exiting." << std::endl;
        return 0;
      }
    }
    else if(jetColl == "AntiKt6LCTopo"){
      if( !(event->retrieve( jets, "AntiKt6LCTopoJets" ).isSuccess()) ){
        std::cout << "Failed to retrieve AntiKt6LCTopoJets container. Exiting." << std::endl;
        return 0;
      }
    }
    else if(jetColl == "AntiKt7LCTopo"){
      if( !(event->retrieve( jets, "AntiKt7LCTopoJets" ).isSuccess()) ){
        std::cout << "Failed to retrieve AntiKt7LCTopoJets container. Exiting." << std::endl;
        return 0;
      }
    }
    else if(jetColl == "AntiKt8LCTopo"){
      if( !(event->retrieve( jets, "AntiKt8LCTopoJets" ).isSuccess()) ){
        std::cout << "Failed to retrieve AntiKt8LCTopoJets container. Exiting." << std::endl;
        return 0;
      }
    }
    else if(jetColl == "AntiKt10LCTopoTrimmedPtFrac5SmallR30"){
      if( !(event->retrieve( jets, "AntiKt10LCTopoTrimmedPtFrac5SmallR30Jets" ).isSuccess()) ){
        std::cout << "Failed to retrieve AntiKt10LCTopoTrimmedPtFrac5SmallR30Jets container. Exiting." << std::endl;
        return 0;
      }
    }

    // Shallow copy 
    std::pair< xAOD::JetContainer*, xAOD::ShallowAuxContainer* > jets_shallowCopy = xAOD::shallowCopyContainer( *jets );

    // Iterate over the shallow copy
    xAOD::JetContainer::iterator jetSC_itr = (jets_shallowCopy.first)->begin();
    xAOD::JetContainer::iterator jetSC_end = (jets_shallowCopy.first)->end();
    for( ; jetSC_itr != jetSC_end; ++jetSC_itr ) {
      m_JetCalibrationTool->applyCalibration(**jetSC_itr);
      // Do something
    }
    delete jets_shallowCopy.first;
    delete jets_shallowCopy.second;
  
  }//END: Loop over events

  delete event;
  delete m_JetCalibrationTool;
  
  xAOD::IOStats::instance().stats().printSmartSlimmingBranchList();

  return 0;

}

#endif
