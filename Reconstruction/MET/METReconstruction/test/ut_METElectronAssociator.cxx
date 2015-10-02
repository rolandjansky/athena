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
#define BOOST_TEST_MODULE TEST_MET_ELECTRON_ASSOCIATOR
#include <boost/test/unit_test.hpp>

#include "TFile.h"
#include "xAODMissingET/MissingETAuxContainer.h"
#include "xAODMissingET/MissingETAssociationMap.h"
#include "xAODMissingET/MissingETAuxAssociationMap.h"
#include "xAODMissingET/MissingETContainer.h"
#include "xAODCaloEvent/CaloClusterChangeSignalState.h"
#include "InDetTrackSelectionTool/InDetTrackSelectionTool.h"
#include "xAODJet/JetContainer.h"

#include "METReconstruction/METJetAssocTool.h"

#define private public //ugly hacking way to do this...
#define protected public
#include "METReconstruction/METElectronAssociator.h"
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
//"/afs/cern.ch/work/r/rsmith/public/METUtilities_testfiles/valid1.110401.PowhegPythia_P2012_ttbar_nonallhad.recon.AOD.e3099_s1982_s1964_r6006_tid04628718_00/AOD.04628718._000158.pool.root.1";

    ifile = new TFile( fileName, "READ" ) ;
    event = new xAOD::TEvent( ifile,  xAOD::TEvent::kClassAccess );
    store = new xAOD::TStore;

    assert( event->readFrom( ifile, xAOD::TEvent::kClassAccess  ) );
  }

  ~globalxAODSetup(){
    delete ifile;
    delete event;
    delete store;
  }
};

struct perTestSetup
{
  met::METElectronAssociator tool;

  InDet::InDetTrackSelectionTool *trackTool;
  met::METJetAssocTool *jetTool;


  perTestSetup() : tool(toolname)
  {
    BOOST_TEST_MESSAGE("starting test" );
    //tool.msg().setLevel(MSG::VERBOSE);//if you are failing tests, this is helpful

    trackTool  = new InDet::InDetTrackSelectionTool("trackSel_RCREC");
    assert(trackTool->setProperty("CutLevel","TightPrimary"));
    assert(trackTool->setProperty("maxZ0SinTheta",1.5));
    assert(trackTool->setProperty("maxD0overSigmaD0",3.0));
    assert(trackTool->initialize());

    jetTool = new met::METJetAssocTool("JetTool_RCREC");

    tool.msg().setLevel(MSG::WARNING);
  }

  ~perTestSetup()
  {
    delete jetTool;
    delete trackTool;
    BOOST_TEST_MESSAGE("ending test");
  }
};

BOOST_GLOBAL_FIXTURE( globalxAODSetup )

BOOST_FIXTURE_TEST_SUITE(Test_Met_Electron_Association_Tool , perTestSetup)

BOOST_AUTO_TEST_CASE( testInitialize ){

  BOOST_REQUIRE(tool.initialize());
}

BOOST_AUTO_TEST_CASE( testWithPV ){
  BOOST_REQUIRE(tool.setProperty("InputCollection","Electrons"));

  ToolHandle<InDet::IInDetTrackSelectionTool> htrackTool(trackTool);
  BOOST_REQUIRE(tool.setProperty("TrackSelectorTool", htrackTool));
  BOOST_REQUIRE(tool.initialize());

  std::string jettype = "AntiKt4LCTopoJets";
  assert(jetTool->setProperty("InputCollection",jettype));
  assert(jetTool->setProperty("TrackSelectorTool",htrackTool));
  assert(jetTool->initialize());
  ToolHandle<IMETAssocToolBase> hjetTool(jetTool);

  Long64_t ievent = 1;//just check with one event
  BOOST_REQUIRE(event->getEntry(ievent) >= 0 );

  const xAOD::CaloClusterContainer* clusters(0);
  BOOST_REQUIRE(  event->retrieve( clusters,    "CaloCalTopoClusters"    ));
  std::cout << " tool handler" << std::endl;

  xAOD::MissingETAssociationMap* metMap = new xAOD::MissingETAssociationMap();
  xAOD::MissingETAuxAssociationMap* metAuxMap = new xAOD::MissingETAuxAssociationMap();
  metMap->setStore(metAuxMap);

  xAOD::MissingETContainer* metCont = new xAOD::MissingETContainer();
  xAOD::MissingETAuxContainer* metAuxCont = new xAOD::MissingETAuxContainer();
  metCont->setStore(metAuxCont);

  const xAOD::JetContainer * jets = nullptr;
  BOOST_REQUIRE(  event->retrieve( jets,  "AntiKt4EMTopoJets"));

  std::cout <<  "jet pt " <<   jets->front()->rawConstituent(0)->pt() << std::endl;

  BOOST_REQUIRE(jetTool->execute(metCont,metMap));//you have to call this one before
  BOOST_REQUIRE(tool.execute(metCont,metMap));

}

BOOST_AUTO_TEST_CASE( testWithoutTrackColl ){
  BOOST_REQUIRE(tool.setProperty("InputCollection","Electrons"));
  BOOST_REQUIRE(tool.setProperty("TrkColl"        , ""));//shut off tracks
  //todo cleanup


  ToolHandle<InDet::IInDetTrackSelectionTool> htrackTool(trackTool);
  BOOST_REQUIRE(tool.setProperty("TrackSelectorTool", htrackTool));
  BOOST_REQUIRE(tool.initialize());

  std::string jettype = "AntiKt4LCTopoJets";
  assert(jetTool->setProperty("InputCollection",jettype));
  assert(jetTool->setProperty("TrackSelectorTool",htrackTool));
  assert(jetTool->initialize());
  ToolHandle<IMETAssocToolBase> hjetTool(jetTool);

  Long64_t ievent = 1;//just check with one event
  BOOST_REQUIRE(event->getEntry(ievent) >= 0 );

  const xAOD::CaloClusterContainer* clusters(0);
  BOOST_REQUIRE(  event->retrieve( clusters,    "CaloCalTopoClusters"    ));

  xAOD::MissingETAssociationMap* metMap = new xAOD::MissingETAssociationMap();
  xAOD::MissingETAuxAssociationMap* metAuxMap = new xAOD::MissingETAuxAssociationMap();
  metMap->setStore(metAuxMap);

  xAOD::MissingETContainer* metCont = new xAOD::MissingETContainer();
  xAOD::MissingETAuxContainer* metAuxCont = new xAOD::MissingETAuxContainer();
  metCont->setStore(metAuxCont);

  const xAOD::JetContainer * jets = nullptr;
  BOOST_REQUIRE( event->retrieve( jets,  "AntiKt4EMTopoJets"));

  BOOST_REQUIRE( jetTool->execute(metCont,metMap));//you have to call this one before
  BOOST_REQUIRE( tool.execute(metCont,metMap));

}



BOOST_AUTO_TEST_SUITE_END() //Test_Met_Electron_Association_Tool
