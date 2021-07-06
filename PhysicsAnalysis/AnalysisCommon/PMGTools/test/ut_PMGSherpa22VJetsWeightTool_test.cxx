/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ut_PMGSherpa22VJetsWeightTool_test.cxx 780506 2016-10-26 13:30:18Z krasznaa $

// Tool include(s):
#include "AsgTools/StandaloneToolHandle.h"

// xAOD include(s):
#ifdef ROOTCORE
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/TStore.h"
#endif // ROOTCORE

// Interface include(s):
#include "PATInterfaces/IWeightTool.h"

// Local include(s):
#include "PMGTools/PMGSherpa22VJetsWeightTool.h"

// ROOT include(s):
#include "TFile.h"
#include "TString.h"
#include "TSystem.h"

// System include(s):
#include <memory>

using namespace asg::msgUserCode;

int main() {

   ANA_CHECK_SET_TYPE (int); //makes ANA_CHECK return ints if exiting function
#ifdef ROOTCORE
   StatusCode::enableFailure();
   ANA_CHECK (xAOD::Init ());
#endif


   asg::StandaloneToolHandle<IWeightTool> myTool;
   myTool.setTypeAndName("PMGTools::PMGSherpa22VJetsWeightTool/PMGSherpa22VJetsWeightTool");
   ANA_CHECK( myTool.setProperty( "TruthJetContainer", "AntiKt4TruthWZJets" ) );  // default
   //   ANA_CHECK( myTool.setProperty( "TruthJetContainer", "AntiKt4TruthJets" ) );
   ANA_CHECK( myTool.retrieve() );


#ifdef ROOTCORE
   const TString fileName = "$ASG_TEST_FILE_MC" ; 
   ANA_MSG_INFO( "Opening file: "
                 << gSystem->ExpandPathName( fileName.Data() ) );

   std::unique_ptr< TFile > ifile( TFile::Open( fileName, "READ" ) );
   xAOD::TEvent event( xAOD::TEvent::kAthenaAccess );
   ANA_CHECK( event.readFrom( ifile.get() ) );

   for( Long64_t ientry = 0; ientry < 10; ++ientry ) {

      if( event.getEntry( ientry ) < 0 ) {
         ANA_MSG_ERROR("Failed to read event " << ientry);
         return -1;
      }

      ANA_MSG_INFO( "Entry " << ientry << ", Event weight = "
                    << myTool->getWeight() ); 
   }
#endif

   return 0; //zero = success
}
