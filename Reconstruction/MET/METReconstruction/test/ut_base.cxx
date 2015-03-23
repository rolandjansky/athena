/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifdef ROOTCORE
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/TStore.h"
#endif

#define BOOST_TEST_MODULE TEST_
#include <boost/test/included/unit_test.hpp>

#include "TFile.h"

#define private public //ugly hacking way to do this...
#define protected public
#include "METReconstruction/METAssociationTool.h"
#undef private
#undef protected

static std::string const toolname = "tool";

struct globalxAODSetup 
{ 
  TFile * ifile;
  xAOD::TEvent * event;
  xAOD::TStore store;


  globalxAODSetup() {
    xAOD::Init() ;                                                         
    // CP::CorrectionCode::enableFailure();
    // StatusCode::enableFailure();                                                                                                                      // CP::SystematicCode::enableFailure();
    // xAOD::TReturnCode::enableFailure();

    TString const fileName = "/afs/cern.ch/work/r/rsmith/public/METUtilities_testfiles/valid1.110401.PowhegPythia_P2012_ttbar_nonallhad.recon.AOD.e3099_s1982_s1964_r6006_tid04628718_00/AOD.04628718._000158.pool.root.1";
   
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
}


BOOST_AUTO_TEST_SUITE_END() //Test_Met_Association_Tool
