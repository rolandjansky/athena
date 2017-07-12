/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// #ifdef XAOD_STANDALONE
// #include "xAODRootAccess/Init.h"
// #include "xAODRootAccess/TEvent.h"
// #include "xAODRootAccess/TStore.h"
// #else
// #include "POOLRootAccess/TEvent.h"
// #include "StoreGate/StoreGateSvc.h"
// #endif

#include <memory.h>

// FrameWork includes
#include "AsgTools/ToolHandle.h"
#include "AsgTools/AsgTool.h"
#include "xAODBase/IParticleContainer.h"
#include "xAODMissingET/MissingETAuxContainer.h"

// #include "TFile.h"
// #include "TSystem.h"

#include "gtest/gtest.h"

#include "METUtilities/METSystematicsTool.h"

namespace met {

  using namespace xAOD;

  const static std::string toolname = "METSystTool";

  class METSystTest : public ::testing::Test
  {
  public:

    METSystTest() :
      tool(toolname)
    {
      tool.msg().setLevel(MSG::WARNING);
    }

//     void SetUpFileInput() {
//       ifile = std::unique_ptr<TFile>( TFile::Open(gSystem->Getenv("ASG_TEST_FILE_MC")) );
// #ifdef XAOD_STANDALONE
//       event = std::unique_ptr<TEvent>( new xAOD::TEvent(TEvent::kClassAccess) );
// #else
//       event = std::unique_ptr<POOL::TEvent>( new POOL::TEvent(POOL::TEvent::kClassAccess) );
// #endif

//       if( event->readFrom( ifile.get() ).isFailure() ) { FAIL(); }

//       Long64_t ievent = 0;//just check with one event
//       if( event->getEntry(ievent)<0 ) {FAIL();};
//     }

  protected:
//     std::unique_ptr<TFile> ifile;
// #ifdef XAOD_STANDALONE
//     std::unique_ptr<TEvent> event;
// #else
//     std::unique_ptr<POOL::TEvent> event;
// #endif

    met::METSystematicsTool tool;

  }; // METSystTest

  TEST_F( METSystTest,  TestDefaultHistosFilled ){
    ASSERT_TRUE(tool.initialize().isSuccess());

    ASSERT_TRUE(tool.m_shiftpara_pthard_njet_mu!=nullptr);
    ASSERT_TRUE(tool.m_resopara_pthard_njet_mu !=nullptr);
    ASSERT_TRUE(tool.m_resoperp_pthard_njet_mu !=nullptr);
    ASSERT_TRUE(tool.m_jet_systRpt_pt_eta      ==nullptr);
  }

  TEST_F( METSystTest,  TestJetTrkHistosFilled ){
    ASSERT_TRUE(tool.setProperty("ConfigJetTrkFile" ,"JetTrackSyst.config"));
    ASSERT_TRUE(tool.initialize().isSuccess());

    ASSERT_TRUE(tool.m_shiftpara_pthard_njet_mu!=nullptr);
    ASSERT_TRUE(tool.m_resopara_pthard_njet_mu !=nullptr);
    ASSERT_TRUE(tool.m_resoperp_pthard_njet_mu !=nullptr);
    ASSERT_TRUE(tool.m_jet_systRpt_pt_eta      !=nullptr);
  }

  TEST_F( METSystTest,  TestNoCollection ){
    ASSERT_TRUE(tool.initialize().isSuccess());
    MissingET softMET("softMet", MissingETBase::Source::softEvent());

    ASSERT_FALSE( asg::CheckHelper<CP::CorrectionCode>::isSuccess( tool.applyCorrection(softMET) ) ) ; //this should fail because the softMET isn't owned by a collection, so we can't applyCorrection on it
  }

  TEST_F( METSystTest,  TestNonSoftTermFailure ){//test source checking

    ASSERT_TRUE(tool.initialize().isSuccess());

    MissingET jetMET(10., 15., 20., "jetMet", MissingETBase::Source::jet());
    MissingET jetMETCopy(jetMET);

    EXPECT_FALSE( asg::CheckHelper<CP::CorrectionCode>::isSuccess( tool.applyCorrection(jetMET) ));//we fail because our object is jetMet
    EXPECT_EQ( jetMETCopy, jetMET ); //if we fail, we don't change our object
    EXPECT_EQ( jetMET.source(), MissingETBase::Source::jet() );//check we don't change the source

  }

  TEST_F( METSystTest,  TestAddAffectingSystematic ){
    ASSERT_TRUE(tool.initialize().isSuccess());

    CP::SystematicSet recSys =  tool.recommendedSystematics();

    CP::SystematicVariation test("testSystematic");//recommendedSystematic

    ASSERT_TRUE( !tool.isAffectedBySystematic(test) );
    ASSERT_TRUE( tool.addAffectingSystematic(test , true /*recommended */) );
    ASSERT_TRUE( tool.isAffectedBySystematic(test) );
  }

  TEST_F( METSystTest,  TestProjectST ){
    ASSERT_TRUE(tool.initialize().isSuccess());

    met::missingEt yaxisSoftTerm (0., 15., 50.);
    met::missingEt xaxisHardTerm (10., 0., 50.);

    met::missingEt proj = tool.projectST(yaxisSoftTerm, xaxisHardTerm);

    EXPECT_FLOAT_EQ(proj.mpx, yaxisSoftTerm.mpx);
    EXPECT_FLOAT_EQ(proj.mpy, (-1)*yaxisSoftTerm.mpy ); //in this case, our "projection" will do a reflection over the x (ptHard) axis
    EXPECT_FLOAT_EQ(proj.sumet, yaxisSoftTerm.sumet);

    met::missingEt projectBack = tool.projectST(proj, xaxisHardTerm);

    EXPECT_FLOAT_EQ( projectBack.mpx, yaxisSoftTerm.mpx );
    EXPECT_FLOAT_EQ( projectBack.mpy, yaxisSoftTerm.mpy );
    EXPECT_FLOAT_EQ( projectBack.sumet, yaxisSoftTerm.sumet );

  }

  TEST_F( METSystTest,  TestProjectST2 ){
    ASSERT_TRUE(tool.initialize().isSuccess());

    met::missingEt yaxisSoftTerm (0., 15., 50.);
    met::missingEt xaxisHardTerm (10., 0., 50.);

    met::missingEt proj = tool.softTrkSyst_scale(yaxisSoftTerm, xaxisHardTerm, 0. );
    EXPECT_FLOAT_EQ( proj.mpx  , yaxisSoftTerm.mpx );
    EXPECT_FLOAT_EQ( proj.mpy  , yaxisSoftTerm.mpy );
    EXPECT_FLOAT_EQ( proj.sumet, yaxisSoftTerm.sumet );

    double shift = 1.;
    proj = tool.softTrkSyst_scale(yaxisSoftTerm, xaxisHardTerm, shift); //
    EXPECT_FLOAT_EQ( proj.mpx  , (yaxisSoftTerm.mpx+shift) );
    EXPECT_FLOAT_EQ( proj.mpy  ,  yaxisSoftTerm.mpy );
    EXPECT_FLOAT_EQ( proj.sumet,  yaxisSoftTerm.sumet );
  }

  TEST_F( METSystTest,  TestNoConfigFiles ){
    ASSERT_TRUE(  tool.setProperty("ConfigSoftTrkFile","") );
    ASSERT_TRUE(  tool.setProperty("ConfigJetTrkFile",""));
    ASSERT_TRUE(  tool.setProperty("ConfigSoftCaloFile",""));

    ASSERT_TRUE(tool.initialize().isSuccess());
  }

  ///////////////////////////////////////////////////////////////////////////////////////////////
  // Tests below are rightfully integration tests, as they require setting up full event reading.
  // Better to replace with some mock tests
  ///////////////////////////////////////////////////////////////////////////////////////////////

  // TEST_F( METSystTest,  TestGetEventInfo)   {
  //   SetUpFileInput();

  //   ASSERT_TRUE( tool.initialize().isSuccess());

  //   ASSERT_NE( tool.getDefaultEventInfo(), nullptr );
  // }

  // TEST_F( METSystTest,  TestSoftTermSuccess ){
  //   SetUpFileInput();

  //   ASSERT_TRUE(tool.initialize().isSuccess());

  //   MissingETContainer testMETcont;
  //   MissingETAuxContainer testMETcontAux;
  //   testMETcont.setStore( testMETcontAux );

  //   MissingET* myObj = new MissingET();
  //   testMETcont.push_back(myObj);
  //   myObj->setName("myMETobj");
  //   myObj->setSource(MissingETBase::Source::softEvent());
  //   ASSERT_EQ( testMETcont.at(0), myObj );//passes
  //   ASSERT_EQ( testMETcont.size(), 1 ); //one element passes

  //   ASSERT_TRUE( asg::CheckHelper<CP::CorrectionCode>::isSuccess( tool.applyCorrection(*myObj) ) );//this time, we should pass because our object is owned
  // }

  // TEST_F( METSystTest,  TestCorrectedCopy ){
  //   SetUpFileInput();
  //   ASSERT_TRUE(tool.initialize().isSuccess());

  //   MissingETContainer    testMETcont;
  //   MissingETAuxContainer testMETcontAux;
  //   testMETcont.setStore( testMETcontAux );

  //   MissingET* myObj  =  new MissingET();
  //   testMETcont.push_back(myObj);
  //   myObj->setName("myMETobj");
  //   myObj->setSource(MissingETBase::Source::softEvent());
  //   ASSERT_EQ( testMETcont.at(0), myObj );
  //   ASSERT_EQ( testMETcont.size(), 1 ); //one element passes

  //   MissingET* correctedCopy = nullptr;
  //   ASSERT_TRUE( asg::CheckHelper<CP::CorrectionCode>::isSuccess( tool.correctedCopy(*myObj, correctedCopy) ) );
  //   ASSERT_NE( correctedCopy, nullptr);

  // }

  // TEST_F( METSystTest,  TestSoftTrkPtHardScale ){
  //   SetUpFileInput();
  //   ASSERT_TRUE(tool.initialize().isSuccess());

  //   CP::SystematicVariation softTrk_ScaleUp("MET_SoftTrk_ScaleUp");
  //   CP::SystematicSet applySys(softTrk_ScaleUp.name());
  //   ASSERT_TRUE(  tool.applySystematicVariation(applySys ) );

  //   MissingETContainer    testMETcont;
  //   MissingETAuxContainer testMETcontAux;
  //   testMETcont.setStore( &testMETcontAux );

  //   MissingET * myObj  =  new MissingET();
  //   testMETcont.push_back(myObj);
  //   myObj->setName("myMETobj");
  //   myObj->setSource(MissingETBase::Source::softEvent());
  //   myObj->setMpx(10.);
  //   myObj->setMpy(15.);
  //   myObj->setSumet(20.);

  //   MissingET * correctedCopy = nullptr;
  //   ASSERT_TRUE( asg::CheckHelper<CP::CorrectionCode>::isSuccess( tool.correctedCopy(*myObj, correctedCopy) ) );

  //   ASSERT_NE(correctedCopy, nullptr);
  //   ASSERT_TRUE( asg::CheckHelper<CP::CorrectionCode>::isSuccess( tool.applyCorrection(*myObj) ) );

  //   EXPECT_FLOAT_EQ( correctedCopy->mpx()  , myObj->mpx() );
  //   EXPECT_FLOAT_EQ( correctedCopy->mpy()  , myObj->mpy() );
  //   EXPECT_FLOAT_EQ( correctedCopy->sumet(), myObj->sumet());

  //   delete correctedCopy;
  // }

  // TEST_F( METSystTest,  TestSoftTrkPtHardResoPara ){
  //   SetUpFileInput();
  //   ASSERT_TRUE(tool.initialize().isSuccess());

  //   CP::SystematicVariation softTrk_ResoPara("MET_SoftTrk_ResoPara");
  //   CP::SystematicSet applySys(softTrk_ResoPara.name());
  //   ASSERT_TRUE(  tool.applySystematicVariation(applySys ) );

  //   MissingETContainer    testMETcont;
  //   MissingETAuxContainer testMETcontAux;
  //   testMETcont.setStore( testMETcontAux );

  //   MissingET * myObj  =  new MissingET();
  //   testMETcont.push_back(myObj);
  //   myObj->setName("myMETobj");
  //   myObj->setSource(MissingETBase::Source::softEvent());
  //   myObj->setMpx(10.);
  //   myObj->setMpy(15.);
  //   myObj->setSumet(20.);

  //   MissingET * correctedCopy = nullptr;
  //   tool.setRandomSeed(199);
  //   ASSERT_TRUE( asg::CheckHelper<CP::CorrectionCode>::isSuccess( tool.correctedCopy(*myObj, correctedCopy) ) );
  //   ASSERT_NE( correctedCopy, nullptr );

  //   tool.setRandomSeed(199);//reset the random seed to redo the same calculation
  //   ASSERT_TRUE( asg::CheckHelper<CP::CorrectionCode>::isSuccess( tool.applyCorrection(*myObj) ) );

  //   EXPECT_FLOAT_EQ( correctedCopy->mpx()  , myObj->mpx() );
  //   EXPECT_FLOAT_EQ( correctedCopy->mpy()  , myObj->mpy() );
  //   EXPECT_FLOAT_EQ( correctedCopy->sumet(), myObj->sumet());

  //   delete correctedCopy;
  // }


  // TEST_F( METSystTest,  TestSoftTrkPtHardResoPerp ){
  //   SetUpFileInput();
  //   ASSERT_TRUE(tool.initialize().isSuccess());

  //   CP::SystematicVariation softTrk_ResoPerp("MET_SoftTrk_ResoPerp");
  //   CP::SystematicSet applySys(softTrk_ResoPerp.name());
  //   ASSERT_TRUE(  tool.applySystematicVariation(applySys ) );

  //   MissingETContainer    testMETcont;
  //   MissingETAuxContainer testMETcontAux;
  //   testMETcont.setStore( testMETcontAux );

  //   MissingET * myObj  =  new MissingET();
  //   testMETcont.push_back(myObj);
  //   myObj->setName("myMETobj");
  //   myObj->setSource(MissingETBase::Source::softEvent());
  //   myObj->setMpx(10.);
  //   myObj->setMpy(15.);
  //   myObj->setSumet(20.);

  //   MissingET * correctedCopy = nullptr;

  //   tool.setRandomSeed(199);
  //   ASSERT_TRUE( asg::CheckHelper<CP::CorrectionCode>::isSuccess( tool.correctedCopy(*myObj, correctedCopy) ) );
  //   ASSERT_NE( correctedCopy, nullptr );

  //   tool.setRandomSeed(199);//reset the random seed to redo the same calculation
  //   ASSERT_TRUE(asg::CheckHelper<CP::CorrectionCode>::isSuccess( tool.applyCorrection(*myObj) ) );

  //   EXPECT_FLOAT_EQ( correctedCopy->mpx()  , myObj->mpx() );
  //   EXPECT_FLOAT_EQ( correctedCopy->mpy()  , myObj->mpy() );
  //   EXPECT_FLOAT_EQ( correctedCopy->sumet(), myObj->sumet() );

  //   delete correctedCopy;
  // }


  // TEST_F( METSystTest,  TestSoftCaloPtScale ){
  //   SetUpFileInput();
  //   ASSERT_TRUE(tool.initialize().isSuccess());

  //   CP::SystematicVariation softCalo_scaleUp("MET_SoftCalo_ScaleUp");
  //   CP::SystematicSet recSys(softCalo_scaleUp.name());
  //   ASSERT_TRUE(  tool.applySystematicVariation(recSys) );

  //   MissingETContainer   testMETcont;
  //   MissingETAuxContainer testMETcontAux;
  //   testMETcont.setStore( testMETcontAux );

  //   MissingET * myObj  =  new MissingET();
  //   testMETcont.push_back(myObj);
  //   myObj->setName("myMETobj");
  //   myObj->setSource(MissingETBase::Source::softEvent());
  //   myObj->setMpx(10.);
  //   myObj->setMpy(15.);
  //   myObj->setSumet(20.);

  //   MissingET * correctedCopy = nullptr;
  //   ASSERT_TRUE( asg::CheckHelper<CP::CorrectionCode>::isSuccess( tool.correctedCopy(*myObj, correctedCopy) ) );
 
  //   ASSERT_NE( correctedCopy, nullptr );
  //   ASSERT_TRUE( asg::CheckHelper<CP::CorrectionCode>::isSuccess( tool.applyCorrection(*myObj) ) );
  //   EXPECT_FLOAT_EQ( correctedCopy->mpx()  , myObj->mpx() );
  //   EXPECT_FLOAT_EQ( correctedCopy->mpy()  , myObj->mpy() );
  //   EXPECT_FLOAT_EQ( correctedCopy->sumet(), myObj->sumet());

  //   delete correctedCopy;
  // }

  // TEST_F( METSystTest,  TestJetTrackMET ){
  //   SetUpFileInput();
  //   ASSERT_TRUE(tool.setProperty("ConfigJetTrkFile" ,"JetTrackSyst.config") );
  //   ASSERT_TRUE(tool.initialize().isSuccess());

  //   CP::SystematicVariation jetTrk_scaleUp("MET_JetTrk_ScaleUp");
  //   CP::SystematicSet recSys(jetTrk_scaleUp.name());
  //   ASSERT_TRUE(  tool.applySystematicVariation(recSys));

  //   MissingETContainer    testMETcont;
  //   MissingETAuxContainer testMETcontAux;
  //   testMETcont.setStore( testMETcontAux );

  //   MissingET * myObj  =  new MissingET();
  //   testMETcont.push_back(myObj);
  //   myObj->setName("myMETobj");
  //   myObj->setSource(MissingETBase::Source::softEvent());
  //   myObj->setMpx(10.);
  //   myObj->setMpy(15.);
  //   myObj->setSumet(20.);

  //   MissingET * correctedCopy = nullptr;
  //   ASSERT_TRUE( tool.correctedCopy(*myObj, correctedCopy));

  //   ASSERT_NE( correctedCopy, nullptr );
  //   ASSERT_TRUE( asg::CheckHelper<CP::CorrectionCode>::isSuccess( tool.applyCorrection(*myObj) ) );
  //   EXPECT_FLOAT_EQ( correctedCopy->mpx()  , myObj->mpx() );
  //   EXPECT_FLOAT_EQ( correctedCopy->mpy()  , myObj->mpy() );
  //   EXPECT_FLOAT_EQ( correctedCopy->sumet(), myObj->sumet() );

  //   delete correctedCopy;
  // }


  // //todo reinclude this when we have the files in PathResolver space
  // TEST_F( METSystTest,  TestPrecommendationFiles) {
  //   SetUpFileInput();
  //   ASSERT_TRUE(  tool.setProperty("ConfigPrefix", "METUtilities/data16_13TeV/rec_July16v1/"));
  //   // ASSERT_TRUE(  tool.setProperty("ConfigSoftTrkFile","TrackSoftTerms.config") );

  //   ASSERT_TRUE(  tool.initialize().isSuccess() ) ;

  //   CP::SystematicVariation softCalo_scaleUp("MET_SoftCalo_ScaleUp");
  //   CP::SystematicSet recSys(softCalo_scaleUp.name());
  //   ASSERT_TRUE(  tool.applySystematicVariation(recSys) );

  //   MissingETContainer    testMETcont;
  //   MissingETAuxContainer testMETcontAux;
  //   testMETcont.setStore( testMETcontAux );

  //   MissingET * myObj  =  new MissingET();
  //   testMETcont.push_back(myObj);
  //   myObj->setName("myMETobj");
  //   myObj->setSource(MissingETBase::Source::softEvent());
  //   myObj->setMpx(10.);
  //   myObj->setMpy(15.);
  //   myObj->setSumet(20.);

  //   MissingET * correctedCopy = nullptr;
  //   ASSERT_TRUE( asg::CheckHelper<CP::CorrectionCode>::isSuccess( tool.correctedCopy(*myObj, correctedCopy) ) );

  //   ASSERT_NE(correctedCopy, nullptr);
  //   ASSERT_TRUE( asg::CheckHelper<CP::CorrectionCode>::isSuccess( tool.applyCorrection(*myObj) ) );
  //   EXPECT_FLOAT_EQ( correctedCopy->mpx()  , myObj->mpx() );
  //   EXPECT_FLOAT_EQ( correctedCopy->mpy()  , myObj->mpy() );
  //   EXPECT_FLOAT_EQ( correctedCopy->sumet(), myObj->sumet() );

  //   delete correctedCopy;
  // }

//   class GaudiEnvironment : public ::testing::Environment {
//   protected:
//     virtual void SetUp() override {

// #ifdef XAOD_STANDALONE
//       xAOD::Init() ;
//       //enable status code failures
//       CP::CorrectionCode::enableFailure();
//       CP::SystematicCode::enableFailure();
//       StatusCode::enableFailure();
//       TReturnCode::enableFailure();
// #else
//       app = POOL::Init(); //important to do this first!
// #endif
//     }
    
//     IAppMgrUI* app;
//   };

}

int main(int argc, char *argv[]){
  // auto g = new met::GaudiEnvironment;
  ::testing::InitGoogleTest( &argc, argv );
  // ::testing::AddGlobalTestEnvironment(g);
  return RUN_ALL_TESTS();
}
