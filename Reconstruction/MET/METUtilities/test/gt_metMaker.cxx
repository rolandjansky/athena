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

#include "METUtilities/METMaker.h"

namespace met {

  using namespace xAOD;

  const static std::string toolname = "METMaker";

  class METMakerTest : public ::testing::Test
  {
  public:

    METMakerTest() :
      tool(toolname)
    {
      tool.msg().setLevel(MSG::WARNING);
    }

//     void SetUpFileInput() {
//       ifile = std::unique_ptr<TFile>( TFile::Open(gSystem->Getenv("ASG_TEST_FILE_MC")) );
// #ifdef XAOD_STANDALONE
//       event = std::unique_ptr<TEvent>( new TEvent(TEvent::kClassAccess) );
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

    met::METMaker tool;

  }; // METMakerTest

  TEST_F( METMakerTest, TestInitialize ){
    ASSERT_TRUE(tool.initialize().isSuccess());
  }

  ///////////////////////////////////////////////////////////////////////////////////////////////
  // Tests below are rightfully integration tests, as they require setting up full event reading.
  // Better to replace with some mock tests
  ///////////////////////////////////////////////////////////////////////////////////////////////

  // TEST_F( METMakerTest, TestSourceTrackMet ){
  //   SetUpFileInput();

  //   ASSERT_TRUE(tool.initialize().isSuccess());

  //   // Create a MissingETContainer with its aux store
  //   MissingETContainer    newMetContainer;
  //   MissingETAuxContainer newMetAuxContainer;
  //   newMetContainer.setStore(newMetAuxContainer);

  //   std::string const jetType = "AntiKt4EMTopo";

  //   const JetContainer* calibJets = nullptr;
  //   ASSERT_TRUE( tool.evtStore()->retrieve(calibJets, jetType + "Jets").isSuccess() );

  //   const MissingETAssociationMap* metMap = nullptr;
  //   ASSERT_TRUE( tool.evtStore()->retrieve(metMap, "METAssoc_" + jetType).isSuccess() );
  //   metMap->resetObjSelectionFlags();

  //   const MissingETContainer* coreMet  = nullptr;
  //   ASSERT_TRUE( tool.evtStore()->retrieve(coreMet, "MET_Core_" + jetType).isSuccess() );

  //   ASSERT_TRUE( tool.rebuildTrackMET("RefJetTrk",
  // 				      "PVSoftTrk",
  // 				      &newMetContainer,
  // 				      calibJets,
  // 				      coreMet,
  // 				      metMap,
  // 				      false
  // 				      ).isSuccess()
  // 		 );


  //   MissingET * jetTrkMet = newMetContainer["RefJetTrk"];
  //   ASSERT_NE( jetTrkMet, nullptr );
  //   ASSERT_TRUE(  MissingETBase::Source::isTrackTerm(jetTrkMet->source()) &&
  // 		  MissingETBase::Source::isJetTerm  (jetTrkMet->source())
  // 		  );
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
