/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifdef ROOTCORE
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/TStore.h"
#endif

// FrameWork includes
#include "AsgTools/ToolHandle.h"
#include "AsgTools/AsgTool.h"
#include "xAODBase/IParticleContainer.h"
#include "xAODMissingET/MissingETAuxContainer.h"

#include "TFile.h"
#include "TSystem.h"

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE TEST_MET_MAKER
#include <boost/test/unit_test.hpp>

#define private public //ugly hacking way to do this...
#define protected public
#include "METUtilities/METSignificance.h"
#undef private
#undef protected

static std::string const toolname = "tool";


struct globalxAODSetup
{
  TFile * ifile;
  xAOD::TEvent * event;
  xAOD::TStore store;


  globalxAODSetup() {
    BOOST_TEST_MESSAGE("Setting up for ut_metSignificanceTool");

    xAOD::Init() ;
    // CP::CorrectionCode::enableFailure();
    // StatusCode::enableFailure();                                                                                                                      // CP::SystematicCode::enableFailure();
    // xAOD::TReturnCode::enableFailure();
    //   TString const fileName = "AOD.pool.root";

    TString const fileName = gSystem->Getenv("ASG_TEST_FILE_MC");

    ifile = new TFile( fileName, "READ" ) ;
    event = new xAOD::TEvent( ifile,  xAOD::TEvent::kClassAccess );
  }

  ~globalxAODSetup(){
    delete event;
    delete ifile;
  }
};

struct perTestSetup
{
  met::METSignificance tool;

  perTestSetup() : tool(toolname)
  {
    BOOST_TEST_MESSAGE("starting test" );
    tool.msg().setLevel(MSG::WARNING);
    // tool.msg().setLevel(MSG::VERBOSE);//if you are failing tests, this is helpful
  }

  ~perTestSetup()
  {
    BOOST_TEST_MESSAGE("ending test");
  }
};

BOOST_GLOBAL_FIXTURE( globalxAODSetup );

BOOST_FIXTURE_TEST_SUITE(Test_Met_Significance , perTestSetup)


BOOST_AUTO_TEST_CASE( testInitialize ){
  BOOST_REQUIRE(tool.initialize());
}

BOOST_AUTO_TEST_CASE( testSourceTrackMet ){
  BOOST_REQUIRE(tool.initialize());

  // Create a MissingETContainer with its aux store
  xAOD::MissingETContainer*    newMetContainer    = new xAOD::MissingETContainer();
  xAOD::MissingETAuxContainer* newMetAuxContainer = new xAOD::MissingETAuxContainer();
  newMetContainer->setStore(newMetAuxContainer);

  const xAOD::MissingETContainer* coreMet  = nullptr;
  std::string const jetType = "AntiKt4EMTopo";
  BOOST_REQUIRE( tool.evtStore()->retrieve(coreMet, "MET_Core_" + jetType) );

  BOOST_REQUIRE( tool.varianceMET(newMetContainer));

  BOOST_REQUIRE( tool.finalize());
  		 

}


BOOST_AUTO_TEST_SUITE_END() //Test_Met_Sigificance
