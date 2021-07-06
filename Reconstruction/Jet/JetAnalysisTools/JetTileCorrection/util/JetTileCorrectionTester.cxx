/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/// @file JetTileCorrectionTester.cxx
/// @brief Contains RootCore testing code for the JetTileCorrectionTool.
///
/// @author Martin Tripiana <tripiana@cern.ch>
///


// System includes
#include <memory>
#include <cstdlib>

// ROOT includes
#include "TFile.h"
#include "TError.h"
#include "TString.h"

// Infrastructure includes
#ifdef ROOTCORE
#   include "xAODRootAccess/Init.h"
#   include "xAODRootAccess/TEvent.h"
#endif // ROOTCORE

// EDM includes
#include "xAODEventInfo/EventInfo.h"
#include "xAODJet/JetContainer.h"
#include "xAODCore/ShallowCopy.h"

#include <AsgTools/StandaloneToolHandle.h>
#include "AsgTools/AsgTool.h"
#include "PATInterfaces/CorrectionCode.h"


// Local includes
#include "JetTileCorrection/JetTileCorrectionTool.h"

using namespace JTC;

//-----------------------------------------------------------------------------
// Error checking macro
//-----------------------------------------------------------------------------
#define CHECK( ARG )                                 \
  do {                                               \
    const bool result = ARG;                         \
    if(!result) {                                    \
      ::Error(APP_NAME, "Failed to execute: \"%s\"", \
              #ARG );                                \
      return 1;                                      \
    }                                                \
  } while( false )


//-----------------------------------------------------------------------------
// Global accessors and decorators
//-----------------------------------------------------------------------------
static SG::AuxElement::Accessor<unsigned int> acc_tileok("TileStatus");
static SG::AuxElement::Accessor<float> acc_ptraw("Ptraw");

//-----------------------------------------------------------------------------
// Main function
//-----------------------------------------------------------------------------
int main( int argc, char* argv[] )
{

  // The application's name
  const char* APP_NAME = argv[ 0 ];

  // Check if we received a file name
  if(argc < 2) {
    Error( APP_NAME, "No file name received!" );
    Error( APP_NAME, "  Usage: %s [xAOD file name] [num events]", APP_NAME );
    return 1;
  }

  // Initialise the application
  CHECK( xAOD::Init(APP_NAME) );
  StatusCode::enableFailure();

  // Open the input file
  const TString fileName = argv[ 1 ];
  Info(APP_NAME, "Opening file: %s", fileName.Data());
  std::unique_ptr<TFile> ifile(TFile::Open(fileName, "READ"));
  CHECK( ifile.get() );

  // Create a TEvent object
  xAOD::TEvent event(xAOD::TEvent::kAthenaAccess);
  xAOD::TStore store;
  CHECK( event.readFrom(ifile.get()) );
  Info(APP_NAME, "Number of events in the file: %i",
       static_cast<int>(event.getEntries()));

  // Decide how many events to run over
  Long64_t entries = event.getEntries();
  if(argc > 2) {
    const Long64_t e = atoll(argv[2]);
    entries = std::min(e, entries);
  }

  // Initialize the tool
  asg::StandaloneToolHandle<CP::IJetTileCorrectionTool> tool_jtc;
  tool_jtc.setTypeAndName("CP::JetTileCorrectionTool/JetTileCorrectionTool");
  
  //Set properties if needed
  CHECK( tool_jtc.setProperty("CorrectionFileName", "JetTileCorrection/JetTile_pFile_010216.root") ); //default anyway
  // std::vector<std::string> dead_modules = {"1 04","1 05","1 06","1 07","1 08","1 09",
  // 					   "2 04","2 05","2 06","2 07","2 08","2 09"  }; // LBA/C5-10 :  NOT REAL SCENARIO !! just trying to get some magnified effect for testing! 
  // CHECK( tool_jtc.setProperty("UserMaskedRegions", dead_modules));

  CHECK( tool_jtc.retrieve() );

  // Loop over the events
  std::cout << "Starting loop" << std::endl;
  for(Long64_t entry = 0; entry < entries; ++entry){

    event.getEntry(entry);

    // Print some event information for fun
    const xAOD::EventInfo* evtInfo = 0;
    CHECK( event.retrieve(evtInfo, "EventInfo") );
    if ( entry%100==0 ){
      Info(APP_NAME, "===>>>  Processing entry %lli, run %u, event %llu  <<<===",
	   entry, evtInfo->runNumber(), evtInfo->eventNumber());
    }
    
    // Get jets
    const xAOD::JetContainer* jets = 0;
    CHECK( event.retrieve(jets, "AntiKt4EMTopoJets") );

    std::pair<xAOD::JetContainer*, xAOD::ShallowAuxContainer*> shallowcopy =
       xAOD::shallowCopyContainer(*jets);
    std::unique_ptr< xAOD::JetContainer > jets_sc( shallowcopy.first );
    std::unique_ptr< xAOD::ShallowAuxContainer >
       jets_scaux( shallowcopy.second );

    for( xAOD::Jet* jet : *jets_sc ){

      if (jet->pt() < 20000. || fabs(jet->eta()) > 2.8) continue;

      //--- apply tile dead module correction
      const CP::CorrectionCode retCode = tool_jtc->applyCorrection(*jet);

      if ( retCode == CP::CorrectionCode::OutOfValidityRange ){
	Warning("JetTileCorrectionTester","No valid pt/eta range. No correction applied."); // It might get too verbosed, just here to illustrate all return values.
      }
      else if( retCode != CP::CorrectionCode::Ok ){
	Error("JetTileCorrectionTester","Failed to apply JetTileCorrection!");
	//return StatusCode::FAILURE;  
      }

      unsigned int j_status = acc_tileok(*jet);

      std::string str_status="";
      if(j_status == (unsigned int)JTC::TS::GOOD)
	str_status = "NotAffected";
      else if(j_status == (unsigned int)JTC::TS::EDGE)
	str_status = "EdgeAffected";
      else if(j_status == (unsigned int)JTC::TS::CORE)
	str_status = "CoreAffected";
      else
	str_status = "Unknown";

      Info(APP_NAME, "Jet status : %s, Pt raw = %.3f GeV, Pt corrected %.3f GeV", str_status.c_str(), acc_ptraw(*jet)*0.001, jet->pt()*0.001);
    }



    // //Check status only 
    // tool_jtc->setRJET(0.1); //change jet radius (for tile status checks only!)
    // for( xAOD::Jet* jet : *jets_sc ){

    //   JTC::TS j_status = tool_jtc->getTileStatus(*jet);

    //   //or well:
    //   //CHECK( tool_jtc->addTileStatus(*jet) );
    //   //unsigned int j_status = acc_tileok(*jet);

    //   std::string str_status="";
    //   if(j_status == JTC::TS::GOOD)
    // 	str_status = "NotAffected";
    //   else if(j_status == JTC::TS::EDGE)
    // 	str_status = "EdgeAffected";
    //   else if(j_status == JTC::TS::CORE)
    // 	str_status = "CoreAffected";
    //   else
    // 	str_status = "Unknown";

    //   Info(APP_NAME, "Jet status : %s, Pt raw = %.3f GeV, Pt corrected %.3f GeV", str_status.c_str(), acc_ptraw(*jet)*0.001, jet->pt()*0.001);
      
    // }    
    // //back to default
    // tool_jtc->setRJET(0.4);
    
  }

  Info(APP_NAME, "Application finished successfully");

  return 0;
}
