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
#define BOOST_TEST_MODULE TEST_MET_SYSTEMATICS_TOOL
#include <boost/test/unit_test.hpp>

#define private public //ugly hacking way to do this...
#define protected public
#include "METUtilities/METSystematicsTool.h"
#undef private
#undef protected

static std::string const toolname = "tool";
static TFile * ifile;
static xAOD::TEvent * event;
static xAOD::TStore *  store;


struct globalxAODSetup
{

  globalxAODSetup() {
    BOOST_TEST_MESSAGE("Setting up for ut_metSystematicsTool");

    xAOD::Init() ;
    CP::CorrectionCode::enableFailure();
    StatusCode::enableFailure();                                                                                                                      // CP::SystematicCode::enableFailure();
    xAOD::TReturnCode::enableFailure();
    // TString const fileName = "AOD.pool.root";

    TString const fileName = gSystem->Getenv("ASG_TEST_FILE_MC");

    ifile = new TFile( fileName, "READ" ) ;
    event = new xAOD::TEvent( ifile,  xAOD::TEvent::kClassAccess );
    store = new xAOD::TStore;
  }

  ~globalxAODSetup(){
    delete event;
    delete ifile;
  }
};

struct perTestSetup
{
  met::METSystematicsTool tool;

  perTestSetup() : tool(toolname)
  {
    BOOST_TEST_MESSAGE("starting test" );
    tool.msg().setLevel(MSG::WARNING);
    // tool.msg().setLevel(MSG::VERBOSE);//if you are failing tests, this is helpful

    Long64_t ievent = 1;//just check with one event
    BOOST_REQUIRE(event->getEntry(ievent) >= 0 );
  }

  ~perTestSetup()
  {
    BOOST_TEST_MESSAGE("ending test");
  }
};

BOOST_GLOBAL_FIXTURE( globalxAODSetup );

BOOST_FIXTURE_TEST_SUITE(Test_Met_Systematics_Tool , perTestSetup)

BOOST_AUTO_TEST_CASE(testDynamicCast)
{
  xAOD::MissingET * testMET = new xAOD::MissingET("myMET");

  const xAOD::IParticleContainer* cont =   dynamic_cast<const xAOD::IParticleContainer*>(testMET->container());
  BOOST_REQUIRE(cont == nullptr );
  const xAOD::MissingETContainer* mcont =  dynamic_cast<const xAOD::MissingETContainer*>(cont);
  BOOST_REQUIRE(mcont == nullptr);
  const xAOD::MissingETContainer* cont2 =  dynamic_cast<const xAOD::MissingETContainer*>(testMET->container());
  BOOST_REQUIRE(cont2 == nullptr);

  delete testMET;

}

BOOST_AUTO_TEST_CASE( testAddObjToCont ){

  xAOD::MissingETContainer * testMETcont  = new xAOD::MissingETContainer();
  xAOD::MissingETAuxContainer* testMETcontAux = new xAOD::MissingETAuxContainer();
  testMETcont->setStore( testMETcontAux );
  BOOST_REQUIRE(testMETcont != nullptr);//passes

  xAOD::MissingET * myObj  =  new xAOD::MissingET();
  testMETcont->push_back(myObj);
  myObj->setName("myMETobj");
  BOOST_REQUIRE(testMETcont->at(0) == myObj);//passes
  BOOST_REQUIRE(testMETcont->size() == 1); //one element passes


  const xAOD::MissingETContainer* cont = dynamic_cast<const xAOD::MissingETContainer*>(myObj->container());
  BOOST_REQUIRE(cont != nullptr );
  BOOST_REQUIRE(cont == testMETcont);
  BOOST_REQUIRE(*cont == *testMETcont);

  delete testMETcont;
}

BOOST_AUTO_TEST_CASE( testDefaultHistosFilled ){
  BOOST_REQUIRE(tool.initialize());

  BOOST_REQUIRE(tool.m_shiftpara_pthard_njet_mu!=nullptr);
  BOOST_REQUIRE(tool.m_resopara_pthard_njet_mu !=nullptr);
  BOOST_REQUIRE(tool.m_resoperp_pthard_njet_mu !=nullptr);
  BOOST_REQUIRE(tool.m_jet_systRpt_pt_eta      ==nullptr);
}

BOOST_AUTO_TEST_CASE( testJetTrkHistosFilled ){
  BOOST_REQUIRE(tool.setProperty("ConfigJetTrkFile" ,"JetTrackSyst.config"));
  BOOST_REQUIRE(tool.initialize());

  BOOST_REQUIRE(tool.m_shiftpara_pthard_njet_mu!=nullptr);
  BOOST_REQUIRE(tool.m_resopara_pthard_njet_mu !=nullptr);
  BOOST_REQUIRE(tool.m_resoperp_pthard_njet_mu !=nullptr);
  BOOST_REQUIRE(tool.m_jet_systRpt_pt_eta      !=nullptr);
}

BOOST_AUTO_TEST_CASE( testNoCollection ){
  BOOST_REQUIRE(tool.initialize());
  xAOD::MissingET softMET("softMet", MissingETBase::Source::softEvent());

  BOOST_REQUIRE( ! tool.applyCorrection(softMET)) ; //this should fail because the softMET isn't owned by a collection, so we can't applyCorrection on it
}

BOOST_AUTO_TEST_CASE( testNonSoftTermFailure ){//test source checking

  BOOST_REQUIRE(tool.initialize());

  xAOD::MissingET jetMET(10., 15., 20., "jetMet", MissingETBase::Source::jet());
  xAOD::MissingET jetMETCopy(jetMET);

  BOOST_REQUIRE( !tool.applyCorrection(jetMET));//we fail because our object is jetMet
  BOOST_REQUIRE( jetMETCopy == jetMET); //if we fail, we don't change our object
  BOOST_REQUIRE(jetMET.source() == MissingETBase::Source::jet());//check we don't change the source



}

BOOST_AUTO_TEST_CASE( testGetEventInfo)   {
  BOOST_REQUIRE(tool.initialize());

  BOOST_REQUIRE(tool.getDefaultEventInfo() );
}

BOOST_AUTO_TEST_CASE( testSoftTermSuccess ){

  BOOST_REQUIRE(tool.initialize());

  xAOD::MissingETContainer   * testMETcont    = new xAOD::MissingETContainer();
  xAOD::MissingETAuxContainer* testMETcontAux = new xAOD::MissingETAuxContainer();
  testMETcont->setStore( testMETcontAux );
  BOOST_REQUIRE(testMETcont != nullptr);//passes

  xAOD::MissingET * myObj = new xAOD::MissingET();
  testMETcont->push_back(myObj);
  myObj->setName("myMETobj");
  myObj->setSource(MissingETBase::Source::softEvent());
  BOOST_REQUIRE(testMETcont->at(0) == myObj);//passes
  BOOST_REQUIRE(testMETcont->size() == 1); //one element passes

  BOOST_REQUIRE( tool.applyCorrection(*myObj));//this time, we should pass because our object is owned

  delete testMETcontAux;
  delete testMETcont;

}

BOOST_AUTO_TEST_CASE( testRetrieveMETTerms ){
  BOOST_REQUIRE(tool.initialize());

  xAOD::MissingETContainer const * cont = nullptr;

  BOOST_REQUIRE(  tool.evtStore()->retrieve(cont,"MET_Reference_AntiKt4EMTopo") );

  xAOD::MissingET const * refgamma = (*cont)["RefGamma"];

  BOOST_REQUIRE(refgamma != nullptr);//we can get met terms out
}


BOOST_AUTO_TEST_CASE( testAddAffectingSystematic ){
  BOOST_REQUIRE(tool.initialize());

  CP::SystematicSet recSys =  tool.recommendedSystematics();

  CP::SystematicVariation test("testSystematic");//recommendedSystematic

  BOOST_REQUIRE( !tool.isAffectedBySystematic(test));
  BOOST_REQUIRE( tool.addAffectingSystematic(test , true /*recommended */) );
  BOOST_REQUIRE( tool.isAffectedBySystematic(test));
}

BOOST_AUTO_TEST_CASE( testProjectST ){
  BOOST_REQUIRE(tool.initialize());

  met::missingEt yaxisSoftTerm (0., 15., 50.);
  met::missingEt xaxisHardTerm (10., 0., 50.);

  met::missingEt proj = tool.projectST(yaxisSoftTerm, xaxisHardTerm);

  BOOST_REQUIRE(proj.mpx == yaxisSoftTerm.mpx &&
	 proj.mpy == (-1)*yaxisSoftTerm.mpy && //in this case, our "projection" will do a reflection over the x (ptHard) axis
	 proj.sumet== yaxisSoftTerm.sumet
	 );

  met::missingEt projectBack = tool.projectST(proj, xaxisHardTerm);

  BOOST_REQUIRE(projectBack.mpx == yaxisSoftTerm.mpx);
  BOOST_REQUIRE(projectBack.mpy == yaxisSoftTerm.mpy);
  BOOST_REQUIRE(projectBack.sumet == yaxisSoftTerm.sumet);

}

BOOST_AUTO_TEST_CASE( testProjectST2 ){
  BOOST_REQUIRE(tool.initialize());

  met::missingEt yaxisSoftTerm (0., 15., 50.);
  met::missingEt xaxisHardTerm (10., 0., 50.);

  met::missingEt proj = tool.softTrkSyst_scale(yaxisSoftTerm, xaxisHardTerm, 0. );
  BOOST_REQUIRE(proj.mpx  == yaxisSoftTerm.mpx);
  BOOST_REQUIRE(proj.mpy  == yaxisSoftTerm.mpy);
  BOOST_REQUIRE(proj.sumet  == yaxisSoftTerm.sumet);

  double shift = 1.;
  proj = tool.softTrkSyst_scale(yaxisSoftTerm, xaxisHardTerm, shift); //
  BOOST_REQUIRE(proj.mpx  == (yaxisSoftTerm.mpx+shift)  &&
		proj.mpy  ==  yaxisSoftTerm.mpy         &&
		proj.sumet==  yaxisSoftTerm.sumet
		);
}

BOOST_AUTO_TEST_CASE( testCorrectedCopy ){
  BOOST_REQUIRE(tool.initialize());

  xAOD::MissingETContainer *   testMETcont    = new xAOD::MissingETContainer();
  xAOD::MissingETAuxContainer* testMETcontAux = new xAOD::MissingETAuxContainer();
  testMETcont->setStore( testMETcontAux );
  BOOST_REQUIRE(testMETcont != nullptr);//passes

  xAOD::MissingET * myObj  =  new xAOD::MissingET();
  testMETcont->push_back(myObj);
  myObj->setName("myMETobj");
  myObj->setSource(MissingETBase::Source::softEvent());
  BOOST_REQUIRE(testMETcont->at(0) == myObj);
  BOOST_REQUIRE(testMETcont->size() == 1); //one element passes

  xAOD::MissingET * correctedCopy = nullptr;
  BOOST_REQUIRE( tool.correctedCopy(*myObj, correctedCopy));
  BOOST_REQUIRE( correctedCopy != nullptr);

  delete testMETcont;
  delete testMETcontAux;


}

BOOST_AUTO_TEST_CASE( testRetrieveTruth ){
  xAOD::MissingETContainer * testMETcont  = new xAOD::MissingETContainer();
  BOOST_REQUIRE(testMETcont != nullptr);

  xAOD::MissingET * met = (*testMETcont)["NonInt"];
  BOOST_REQUIRE(met == nullptr );

  delete testMETcont;
}

BOOST_AUTO_TEST_CASE( testSoftTrkPtHardScale ){
  BOOST_REQUIRE(tool.initialize());

  CP::SystematicVariation softTrk_ScaleUp("MET_SoftTrk_ScaleUp");
  CP::SystematicSet applySys(softTrk_ScaleUp.name());
  BOOST_REQUIRE(  tool.applySystematicVariation(applySys ) );

  xAOD::MissingETContainer *   testMETcont    = new xAOD::MissingETContainer();
  xAOD::MissingETAuxContainer* testMETcontAux = new xAOD::MissingETAuxContainer();
  testMETcont->setStore( testMETcontAux );
  BOOST_REQUIRE(testMETcont != nullptr);//passes

  xAOD::MissingET * myObj  =  new xAOD::MissingET();
  testMETcont->push_back(myObj);
  myObj->setName("myMETobj");
  myObj->setSource(MissingETBase::Source::softEvent());
  myObj->setMpx(10.);
  myObj->setMpy(15.);
  myObj->setSumet(20.);

  xAOD::MissingET * correctedCopy = nullptr;
  BOOST_REQUIRE( tool.correctedCopy(*myObj, correctedCopy));

  BOOST_REQUIRE(correctedCopy != nullptr);
  BOOST_REQUIRE(tool.applyCorrection(*myObj));
  BOOST_REQUIRE( (int)correctedCopy->mpx()  ==  (int)myObj->mpx() &&
	  (int)correctedCopy->mpy()         ==  (int)myObj->mpy() &&
	  (int)correctedCopy->sumet()       ==  (int)myObj->sumet());


  delete testMETcont;
  delete testMETcontAux;
}

BOOST_AUTO_TEST_CASE( testSoftTrkPtHardResoPara ){
  BOOST_REQUIRE(tool.initialize());

  CP::SystematicVariation softTrk_ResoPara("MET_SoftTrk_ResoPara");
  CP::SystematicSet applySys(softTrk_ResoPara.name());
  BOOST_REQUIRE(  tool.applySystematicVariation(applySys ) );

  xAOD::MissingETContainer *   testMETcont    = new xAOD::MissingETContainer();
  xAOD::MissingETAuxContainer* testMETcontAux = new xAOD::MissingETAuxContainer();
  testMETcont->setStore( testMETcontAux );
  BOOST_REQUIRE(testMETcont != nullptr);//passes

  xAOD::MissingET * myObj  =  new xAOD::MissingET();
  testMETcont->push_back(myObj);
  myObj->setName("myMETobj");
  myObj->setSource(MissingETBase::Source::softEvent());
  myObj->setMpx(10.);
  myObj->setMpy(15.);
  myObj->setSumet(20.);

  xAOD::MissingET * correctedCopy = nullptr;
  tool.setRandomSeed(199);
  BOOST_REQUIRE( tool.correctedCopy(*myObj, correctedCopy));
  BOOST_REQUIRE(correctedCopy != nullptr);

  tool.setRandomSeed(199);//reset the random seed to redo the same calculation
  BOOST_REQUIRE(tool.applyCorrection(*myObj));

  BOOST_REQUIRE( (int)correctedCopy->mpx()  ==  (int)myObj->mpx() &&
	  (int)correctedCopy->mpy()  ==  (int)myObj->mpy() &&
	  (int)correctedCopy->sumet()  ==  (int)myObj->sumet());

  delete testMETcontAux;
  delete testMETcont;
}


BOOST_AUTO_TEST_CASE( testSoftTrkPtHardResoPerp ){
  BOOST_REQUIRE(tool.initialize());

  CP::SystematicVariation softTrk_ResoPerp("MET_SoftTrk_ResoPerp");
  CP::SystematicSet applySys(softTrk_ResoPerp.name());
  BOOST_REQUIRE(  tool.applySystematicVariation(applySys ) );

  xAOD::MissingETContainer *   testMETcont    = new xAOD::MissingETContainer();
  xAOD::MissingETAuxContainer* testMETcontAux = new xAOD::MissingETAuxContainer();
  testMETcont->setStore( testMETcontAux );
  BOOST_REQUIRE(testMETcont != nullptr);//passes

  xAOD::MissingET * myObj  =  new xAOD::MissingET();
  testMETcont->push_back(myObj);
  myObj->setName("myMETobj");
  myObj->setSource(MissingETBase::Source::softEvent());
  myObj->setMpx(10.);
  myObj->setMpy(15.);
  myObj->setSumet(20.);

  xAOD::MissingET * correctedCopy = nullptr;

  tool.setRandomSeed(199);
  BOOST_REQUIRE( tool.correctedCopy(*myObj, correctedCopy));
  BOOST_REQUIRE(correctedCopy != nullptr);

  tool.setRandomSeed(199);//reset the random seed to redo the same calculation
  BOOST_REQUIRE(tool.applyCorrection(*myObj));

  BOOST_REQUIRE( (int)correctedCopy->mpx()  ==  (int)myObj->mpx() &&
	  (int)correctedCopy->mpy()  ==  (int)myObj->mpy() &&
	  (int)correctedCopy->sumet()  ==  (int)myObj->sumet());

  delete testMETcont;
  delete testMETcontAux;
}



BOOST_AUTO_TEST_CASE( testSoftCaloPtScale ){
  BOOST_REQUIRE(tool.initialize());

  CP::SystematicVariation softCalo_scaleUp("MET_SoftCalo_ScaleUp");
  CP::SystematicSet recSys(softCalo_scaleUp.name());
  BOOST_REQUIRE(  tool.applySystematicVariation(recSys) );

  xAOD::MissingETContainer *   testMETcont    = new xAOD::MissingETContainer();
  xAOD::MissingETAuxContainer* testMETcontAux = new xAOD::MissingETAuxContainer();
  testMETcont->setStore( testMETcontAux );
  BOOST_REQUIRE(testMETcont != nullptr);//passes

  xAOD::MissingET * myObj  =  new xAOD::MissingET();
  testMETcont->push_back(myObj);
  myObj->setName("myMETobj");
  myObj->setSource(MissingETBase::Source::softEvent());
  myObj->setMpx(10.);
  myObj->setMpy(15.);
  myObj->setSumet(20.);

  xAOD::MissingET * correctedCopy = nullptr;
  BOOST_REQUIRE( tool.correctedCopy(*myObj, correctedCopy));

  BOOST_REQUIRE(correctedCopy != nullptr);
  BOOST_REQUIRE(tool.applyCorrection(*myObj));
  BOOST_REQUIRE( (int)correctedCopy->mpx()  ==  (int)myObj->mpx() &&
	  (int)correctedCopy->mpy()  ==  (int)myObj->mpy() &&
	  (int)correctedCopy->sumet()  ==  (int)myObj->sumet());

  delete testMETcont;
  delete testMETcontAux;
  delete correctedCopy;
}

BOOST_AUTO_TEST_CASE( testJetTrackMET ){
  BOOST_REQUIRE(tool.setProperty("ConfigJetTrkFile" ,"JetTrackSyst.config") );
  BOOST_REQUIRE(tool.initialize());

  CP::SystematicVariation jetTrk_scaleUp("MET_JetTrk_ScaleUp");
  CP::SystematicSet recSys(jetTrk_scaleUp.name());
  BOOST_REQUIRE(  tool.applySystematicVariation(recSys));

  xAOD::MissingETContainer *   testMETcont    = new xAOD::MissingETContainer();
  xAOD::MissingETAuxContainer* testMETcontAux = new xAOD::MissingETAuxContainer();
  testMETcont->setStore( testMETcontAux );
  BOOST_REQUIRE(testMETcont != nullptr);//passes

  xAOD::MissingET * myObj  =  new xAOD::MissingET();
  testMETcont->push_back(myObj);
  myObj->setName("myMETobj");
  myObj->setSource(MissingETBase::Source::softEvent());
  myObj->setMpx(10.);
  myObj->setMpy(15.);
  myObj->setSumet(20.);

  xAOD::MissingET * correctedCopy = nullptr;
  BOOST_REQUIRE( tool.correctedCopy(*myObj, correctedCopy));

  BOOST_REQUIRE(correctedCopy != nullptr);
  BOOST_REQUIRE(tool.applyCorrection(*myObj));
  BOOST_REQUIRE( (int)correctedCopy->mpx()  ==  (int)myObj->mpx() &&
	  (int)correctedCopy->mpy()  ==  (int)myObj->mpy() &&
	  (int)correctedCopy->sumet()  ==  (int)myObj->sumet());

  delete testMETcont;
  delete testMETcontAux;
  delete correctedCopy;
}


BOOST_AUTO_TEST_CASE( testNoConfigFiles ){
  BOOST_REQUIRE(  tool.setProperty("ConfigSoftTrkFile","") );
  BOOST_REQUIRE(  tool.setProperty("ConfigJetTrkFile",""));
  BOOST_REQUIRE(  tool.setProperty("ConfigSoftCaloFile",""));

  BOOST_REQUIRE(tool.initialize());


}

//todo reinclude this when we have the files in PathResolver space
BOOST_AUTO_TEST_CASE( testPrecommendationFiles) {
  BOOST_REQUIRE(  tool.setProperty("ConfigPrefix", "METUtilities/data16_13TeV/rec_July16v1/"));
  // BOOST_REQUIRE(  tool.setProperty("ConfigSoftTrkFile","TrackSoftTerms.config") );

  BOOST_REQUIRE(  tool.initialize() ) ;

  CP::SystematicVariation softCalo_scaleUp("MET_SoftCalo_ScaleUp");
  CP::SystematicSet recSys(softCalo_scaleUp.name());
  BOOST_REQUIRE(  tool.applySystematicVariation(recSys) );

  xAOD::MissingETContainer *   testMETcont    = new xAOD::MissingETContainer();
  xAOD::MissingETAuxContainer* testMETcontAux = new xAOD::MissingETAuxContainer();
  testMETcont->setStore( testMETcontAux );
  BOOST_REQUIRE(testMETcont != nullptr);//passes

  xAOD::MissingET * myObj  =  new xAOD::MissingET();
  testMETcont->push_back(myObj);
  myObj->setName("myMETobj");
  myObj->setSource(MissingETBase::Source::softEvent());
  myObj->setMpx(10.);
  myObj->setMpy(15.);
  myObj->setSumet(20.);

  xAOD::MissingET * correctedCopy = nullptr;
  BOOST_REQUIRE( tool.correctedCopy(*myObj, correctedCopy));

  BOOST_REQUIRE(correctedCopy != nullptr);
  BOOST_REQUIRE(tool.applyCorrection(*myObj));
  BOOST_REQUIRE( (int)correctedCopy->mpx()  ==  (int)myObj->mpx() &&
		 (int)correctedCopy->mpy()  ==  (int)myObj->mpy() &&
		 (int)correctedCopy->sumet()  ==  (int)myObj->sumet());

  delete testMETcont;
  delete testMETcontAux;
  delete correctedCopy;
}


BOOST_AUTO_TEST_SUITE_END() //Test_Met_Systematics_Tool
