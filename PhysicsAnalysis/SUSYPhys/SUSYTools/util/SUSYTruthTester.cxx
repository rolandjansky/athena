/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <memory>
#include <cstdlib>
#include <string>
#include <iostream>

// ROOT include(s):
#include <TFile.h>
#include <TError.h>
#include <TString.h>
#include <TStopwatch.h>
#include <TSystem.h>
#include "TObjArray.h"
#include "TObjString.h"

#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TStore.h"
#ifdef ROOTCORE
#include "xAODRootAccess/TEvent.h"
#else
#include "POOLRootAccess/TEvent.h"
#endif // ROOTCORE
#include "PATInterfaces/CorrectionCode.h"

// EDM include(s):
#include "xAODEventInfo/EventInfo.h"
#include "xAODTruth/TruthParticleAuxContainer.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthEventContainer.h"
#include "xAODTruth/TruthEvent.h"
#include "xAODCore/ShallowCopy.h"
#include "xAODBase/IParticleHelpers.h"
#include "xAODTruth/xAODTruthHelpers.h"

// Local include(s):
#include "SUSYTools/SUSYObjDef_xAOD.h"

int main( int argc, char* argv[] ) {

  using namespace asg::msgUserCode;
  ANA_CHECK_SET_TYPE (int);

  //StatusCode::enableFailure();
  StatusCode::enableFailure();
  CP::CorrectionCode::enableFailure();

  // The application's name:
  const char* APP_NAME = argv[ 0 ];

  // Check if we received a file name:
  if ( argc < 2 ) {
    Error( APP_NAME, "No file name received!" );
    Error( APP_NAME, "  Usage: %s [xAOD file name] [maxEvents] [Debug] [ConfigFile=<cfile.conf>] ", APP_NAME );
    return 1;
  }

  /// READ CONFIG ------------
  int debug = 1;
  Long64_t entries=-1;
  std::string config_file = "SUSYTools/SUSYTools_Default.conf";

  for (int i = 1 ; i < argc ; i++) {
    const char* key = strtok(argv[i], "=") ;
    const char* val = strtok(0, " ") ;

    Info( APP_NAME,  "processing key %s  with value %s", key, val );

    if (strcmp(key, "Debug") == 0) debug = atoi(val);
    if (strcmp(key, "ConfigFile") == 0) config_file = std::string(val);
    if (strcmp(key, "maxEvents") == 0) entries = atoi(val);
  }

  ///
  static SG::AuxElement::Accessor<int> acc_susyid("SUSY_procID");

  // Initialise the application:
  //ANA_CHECK( xAOD::Init( APP_NAME ) );  //NOT WORKING? //MT,WB

  // Open the input file:
  TString fileName;
  fileName = argv[1];
  Info( APP_NAME, "Opening file: %s", fileName.Data() );
  std::unique_ptr< TFile > ifile( TFile::Open( fileName, "READ" ) );
  ANA_CHECK( ifile.get() );

  // Create a TEvent object:


#ifdef ROOTCORE
  xAOD::TEvent event( xAOD::TEvent::kAthenaAccess );
#else
  POOL::TEvent event( POOL::TEvent::kAthenaAccess );
#endif

  //xAOD::TEvent event( xAOD::TEvent::kBranchAccess );
  //xAOD::TEvent event( xAOD::TEvent::kClassAccess );
  ANA_CHECK( event.readFrom( ifile.get() ) );
  Info( APP_NAME, "Number of events in the file: %i",
        static_cast< int >( event.getEntries() ) );

  TString xStream=""; //identify SUSY DxAOD flavour   (inside entry loop now /MT)

  xAOD::TStore store;

  // If we haven't set the number of events, then run over the whole tree
  if (entries<0){
    entries = event.getEntries();
  }

  // Create the tool(s) to test:
  ST::SUSYObjDef_xAOD objTool("SUSYObjDef_xAOD");

  int period = debug ? 1 : 100;

  // Loop over the events:
  for ( Long64_t entry = 0; entry < entries; ++entry ) {

    // Tell the object which entry to look at:
    event.getEntry( entry );

    // Print some event information for fun:
    const xAOD::EventInfo* ei = 0;
    ANA_CHECK( event.retrieve( ei, "EventInfo" ) );

    if (entry % period == 0) {
      Info( APP_NAME,
            "===>>>  start processing event #%i, "
            "run #%i %i events processed so far  <<<===",
            static_cast< int >( ei->eventNumber() ),
            static_cast< int >( ei->runNumber() ),
            static_cast< int >( entry ) );
    }

    // Check SUSY Proc. ID for signal MC (only for first event for now!)
    int pdgid1 = 0;
    int pdgid2 = 0;
    
    if( objTool.FindSusyHP(pdgid1, pdgid2) != StatusCode::SUCCESS ){
      Error(APP_NAME, "--- SOMETHING IS WRONG WITH THE SUSY PROC FINDING... ---");
      return StatusCode::FAILURE;
    }
    
    if( pdgid1!=0 && pdgid2!=0){ //(just to avoid warnings)
      Info(APP_NAME , "--- SIGNAL ID1     : %d", pdgid1);
      Info(APP_NAME , "    SIGNAL ID2     : %d", pdgid2);

      if ( acc_susyid.isAvailable(*ei)  ){
        Info(APP_NAME , "    SIGNAL PROC ID (DECO) : %d", acc_susyid(*ei) );
      }
    }
  }

  // CALLGRIND_TOGGLE_COLLECT;
  // CALLGRIND_DUMP_STATS;
  // ProfilerStop();

  // Return gracefully:
  return 0;
}
