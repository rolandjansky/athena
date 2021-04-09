/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ut_PMGSherpaVjetsSysTool_test.cxx 2016-07-13 tripiana $

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
#include "PMGTools/PMGSherpaVjetsSysTool.h"

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


   asg::StandaloneToolHandle< PMGTools::IPMGSherpaVjetsSysTool > myTool;
   myTool.setTypeAndName("PMGTools::PMGSherpaVjetsSysTool/PMGSherpaVjetsSysTool");
   ANA_CHECK( myTool.setProperty( "TruthJetContainer", "AntiKt4TruthJets" ) );  // default
   //   ANA_CHECK( myTool.setProperty( "TruthJetContainer", "AntiKt4TruthJets" ) );
   ANA_CHECK( myTool.initialize() );


#ifdef ROOTCORE
   const TString fileName = "$ASG_TEST_FILE_MC";
   ANA_MSG_INFO( "Opening file: "
                 << gSystem->ExpandPathName( fileName.Data() ) );

   std::unique_ptr< TFile > ifile( TFile::Open( fileName, "READ" ) );
   xAOD::TEvent event( xAOD::TEvent::kAthenaAccess );
   ANA_CHECK( event.readFrom( ifile.get() ) );

   for( Long64_t ientry = 0; ientry < 10; ++ientry ) {

     if(ientry==0) 
       myTool->cacheDSID(true); // cache DSID to speed up look-up

     if( event.getEntry(ientry) < 0 ){
       ANA_MSG_ERROR("Failed to read event " << ientry);
       return -1;
     } 
     
     unsigned int njets = myTool->getNtruthJets();
     myTool->setNjets(njets);

     ANA_MSG_INFO("");
     ANA_MSG_INFO("EVENT " << ientry << " (Njets = " << njets << ")");
     ANA_MSG_INFO("---------------------------------");
     
     for(auto& var : myTool->getSupportedVariations(PMGTools::SysParType::Znunu))
       ANA_MSG_INFO("WEIGHT FOR " << var << " = " << myTool->getWeight(var)); 

   }
#endif
   
   return 0; //zero = success
}
