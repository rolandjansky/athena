/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ut_accessData.cxx 638120 2015-01-07 19:10:02Z sfarrell $

// System include(s):

// ROOT include(s):
#include <TFile.h>

// Infrastructure include(s):
#ifdef ROOTCORE
#   include "xAODRootAccess/Init.h"
#   include "xAODRootAccess/TEvent.h"
#endif // ROOTCORE

// EDM include(s):
#include "xAODJet/JetContainer.h"

// Local include(s):
#include "CPAnalysisExamples/errorcheck.h"

int main( int /*argc*/, char* argv[] ) {

   // The application's name:
   const char* APP_NAME = argv[ 0 ];

   //bool m_debug = true;

   // Initialise the application:
   CHECK( xAOD::Init( APP_NAME ) );

   // Open the input file:
   const TString fileName = "/afs/cern.ch/atlas/project/PAT/xAODs/r5591/mc14_8TeV.177998.MadGraphPythia_AUET2BCTEQ6L1_ttbargammaPt80_noAllHad_fixed.merge.AOD.e2189_s1933_s1911_r5591_r5625_tid01593004_00/AOD.01593004._000018.pool.root.1";

   Info( APP_NAME, "Opening file: %s", fileName.Data() );
   TFile * ifile( TFile::Open( fileName, "READ" ) );
   if (!ifile) Error(APP_NAME, "%s", ("Cannot find file " + fileName).Data());

   // Create a TEvent object:
   xAOD::TEvent event( ifile, xAOD::TEvent::kClassAccess );
   CHECK( event.readFrom( ifile ) );
   Info( APP_NAME, "Number of events in the file: %i",
         static_cast< int >( event.getEntries() ) );

   // Decide how many events to run over:
   Long64_t entries = event.getEntries();

   for( Long64_t entry = 0; entry < entries; ++entry )
   {
     Info( APP_NAME, "Running on event %i", ((int)entry));
      // Tell the object which entry to look at:
      event.getEntry( entry );

      /***************************************/ 
      // test jets

      // Get the Jets from the event:
      const xAOD::JetContainer* jets = 0;
      CHECK( event.retrieve( jets, "AntiKt4LCTopoJets" ) );
   }



   // Return gracefully:
   return 0;
}
