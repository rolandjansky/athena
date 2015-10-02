/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifdef ROOTCORE
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/TStore.h"
#endif

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE TEST_BASE
#include <boost/test/unit_test.hpp>

#include "TFile.h"

#define private public //ugly hacking way to do this...
#define protected public
#include "METReconstruction/METAssociationTool.h"
#undef private
#undef protected

static std::string const toolname = "tool";
static TFile * ifile;
static xAOD::TEvent * event;
static xAOD::TStore * store;

struct globalxAODSetup
{


  globalxAODSetup() {
    xAOD::Init() ;
    // CP::CorrectionCode::enableFailure();
    // StatusCode::enableFailure();                                                                                                                      // CP::SystematicCode::enableFailure();
    // xAOD::TReturnCode::enableFailure();

    TString const fileName = "/afs/cern.ch/work/m/maklein/public/mc14_13TeV.110401.PowhegPythia_P2012_ttbar_nonallhad.merge.AOD.e2928_s1982_s2008_r6114_r6104_tid04859512_00/AOD.04859512._000001.pool.root";

    ifile = new TFile( fileName, "READ" ) ;
    store = new xAOD::TStore;
    event = new xAOD::TEvent( ifile,  xAOD::TEvent::kClassAccess );

  }

  ~globalxAODSetup(){
    delete store;
    delete event;
    delete ifile;
  }
};

struct perTestSetup
{
  met::METAssociationTool tool;

  perTestSetup() : tool(toolname)
  {
    BOOST_TEST_MESSAGE("starting test" );
    //tool.msg().setLevel(MSG::VERBOSE);//if you are failing tests, this is helpful
    tool.msg().setLevel(MSG::WARNING);
  }

  ~perTestSetup()
  {
    BOOST_TEST_MESSAGE("ending test");
  }
};

BOOST_GLOBAL_FIXTURE( globalxAODSetup )

BOOST_FIXTURE_TEST_SUITE(Test_Met_Association_Tool , perTestSetup)


BOOST_AUTO_TEST_CASE( testInitialize ){
  BOOST_REQUIRE(tool.initialize());

  Long64_t ievent = 1;//just check with one event
  BOOST_REQUIRE(event->getEntry(ievent) >= 0 );
}


BOOST_AUTO_TEST_SUITE_END() //Test_Met_Association_Tool
