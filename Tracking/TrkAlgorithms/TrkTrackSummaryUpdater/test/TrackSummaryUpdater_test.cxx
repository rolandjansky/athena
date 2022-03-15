/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
/**
 * @file TrkTrackSummaryUpdater/test/TrackSummaryUpdater_test.cxx
 * @author Shaun Roe
 * @date Nov, 2020
 * @brief Some tests for TrackSummaryUpdater algorithm in the Boost framework
 */
 
 
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE TEST_TRKTRACKSUMMARYUPDATER
//
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Woverloaded-virtual"
#include <boost/test/unit_test.hpp>

#pragma GCC diagnostic pop

//

#include "GaudiKernel/ISvcLocator.h"
#include "StoreGate/StoreGateSvc.h"
#include "CxxUtils/checker_macros.h"
#include "TInterpreter.h"
//
#include "GaudiKernel/IAppMgrUI.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/EventContext.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "CxxUtils/ubsan_suppress.h"
#include "PutTrackCollectionsInSG.h"

#include "TrkToolInterfaces/IExtendedTrackSummaryTool.h"
#include "StoreGate/ReadHandleKey.h"
#include "TrkTrack/TrackCollection.h"
#include "../src/TrackSummaryUpdater.h"
#include "TrkTrackSummary/TrackSummary.h"
#include <vector>
#include "test/SummaryToolStub.h"

ATLAS_NO_CHECK_FILE_THREAD_SAFETY; // This test uses global svcLoc.

// Gaudi fixture
class GaudiFixture {
 public:
   ISvcLocator * 
   svcLoc(){
     return m_svcLoc;
   }
   
   IToolSvc *
   toolSvc(){
     return m_toolSvc;
   }
   
   StoreGateSvc* 
   storeGateSvc(){
    return m_sg;
   }

   GaudiFixture(const std::string & joPath = "TrkTrackSummaryUpdater/TrackSummaryUpdater_test.txt") {
     setUpGaudi(joPath);
   }
 
   ~GaudiFixture() {
     tearDownGaudi();
   }
   
 private:
   void 
   setUpGaudi(const std::string & joPath) {
     CxxUtils::ubsan_suppress ([]() { TInterpreter::Instance(); } );
     m_appMgr = Gaudi::createApplicationMgr();
     m_svcLoc = m_appMgr;
     m_svcMgr = m_appMgr;
     m_propMgr = m_appMgr;
     m_propMgr->setProperty( "EvtSel",         "NONE" ).ignore() ;
     m_propMgr->setProperty( "JobOptionsType", "FILE" ).ignore();
     m_propMgr->setProperty( "JobOptionsPath", joPath ).ignore();
     m_toolSvc = m_svcLoc->service("ToolSvc");
     m_appMgr->configure().ignore();
     m_appMgr->initialize().ignore();
     m_sg = nullptr;
     m_svcLoc->service ("StoreGateSvc", m_sg).ignore();
   }
 
   void 
   tearDownGaudi() {
     m_svcMgr->finalize().ignore();
     m_appMgr->finalize().ignore();
     m_appMgr->terminate().ignore();
     m_svcLoc->release();
     m_svcMgr->release();
     Gaudi::setInstance( static_cast<IAppMgrUI*>(nullptr) );
   }
 
   StoreGateSvc* 
   evtStore(){
     return m_sg;
   }
 
   //member variables for Core Gaudi components
   IAppMgrUI*               m_appMgr{nullptr};
   SmartIF<ISvcLocator>     m_svcLoc;
   SmartIF<ISvcManager>     m_svcMgr;
   SmartIF<IToolSvc>        m_toolSvc;
   SmartIF<IProperty>       m_propMgr;
   StoreGateSvc*            m_sg{ nullptr };
 };
 


 
BOOST_AUTO_TEST_SUITE(TrackSummaryUpdaterTest)
  
  GaudiFixture g("TrkTrackSummaryUpdater/TrackSummaryUpdater_test.txt");
  auto  pSvcLoc=g.svcLoc();
  auto  pToolSvc=g.toolSvc();

  BOOST_AUTO_TEST_CASE( sanityCheck ){
    const bool svcLocatorIsOk=(pSvcLoc != nullptr);
    BOOST_TEST(svcLocatorIsOk);
    const bool toolSvcIsOk = ( pToolSvc != nullptr);
    BOOST_TEST(toolSvcIsOk);
  }
  
  BOOST_AUTO_TEST_CASE(execute){
    auto *pAlg = new PutTrackCollectionsInSG("PutCollectionsInSG",pSvcLoc);
    pAlg->addRef();
    BOOST_TEST(pAlg->execute().isSuccess());
    std::string collectionKey1("StoreGateSvc+TrackCollectionKey1");
    SG::ReadHandle<TrackCollection> thisTrackCollection1 (collectionKey1);
    BOOST_TEST(thisTrackCollection1->size() == 2);
    //
    IAlgTool* toolInterface{};
    //SummaryToolStub is in this package, it is a dummy SummaryTool with the appropriate
    //interface (inheriting from IExtendedTrackSummaryTool)
    BOOST_TEST(pToolSvc->retrieveTool("SummaryToolStub", toolInterface).isSuccess());
    //Create TrackSummary in the output collection
    auto *pUpdateAlg = new Trk::TrackSummaryUpdater("Updater1",pSvcLoc);
    pUpdateAlg->addRef();
    BOOST_TEST_MESSAGE("Testing Summary Creation on the track");
    BOOST_TEST(pUpdateAlg->setProperty("InputTracksKey","TrackCollectionKey1").isSuccess());
    BOOST_TEST(pUpdateAlg->setProperty("OutputTracksKey","SummaryCreation").isSuccess());
    BOOST_TEST(pUpdateAlg->setProperty("SummaryTool", "SummaryToolStub").isSuccess());
    //The following means that "computeAndReplaceTrackSummary" should be called
    BOOST_TEST(pUpdateAlg->setProperty("UpdateSharedHits", false).isSuccess());
    //initialize() is necessary here
    BOOST_TEST(pUpdateAlg->initialize().isSuccess());
    BOOST_TEST(pUpdateAlg->execute().isSuccess());
    std::vector<std::string> keysPresent{};
    g.storeGateSvc()->keys<TrackCollection>(keysPresent);
    BOOST_TEST_MESSAGE("StoreGate now contains the following entries: ");
    for (auto & n: keysPresent){
      BOOST_TEST_MESSAGE(n);
    }
    std::string createdCollectionKey("SummaryCreation");
    SG::ReadHandle<TrackCollection> createdCollection (createdCollectionKey);
    BOOST_TEST(createdCollection->size() == 2);
    //
    const std::vector<std::string> expectedKeys{"SummaryCreation","TrackCollectionKey1", "TrackCollectionKey2"};
    g.storeGateSvc()->keys<TrackCollection>(keysPresent);
    BOOST_TEST(keysPresent == expectedKeys, boost::test_tools::per_element());
    //
    const int valueSetForNHitsUsed{3};
    for (const auto & aTrack: *createdCollection){
      const auto & pThisSummary = aTrack->trackSummary();
      const auto valueReadBackForNHitsUsed = pThisSummary->numberOfUsedHitsdEdx();
      BOOST_TEST(valueReadBackForNHitsUsed == valueSetForNHitsUsed);
    }
    //Update the Additional Info on the existing track summary, using numberOfUsedHitsdEdx
    //as an example of a value to update. The update values are hard coded in the 
    //SummaryToolStub code in this package, which is a mock SummaryTool
    BOOST_TEST_MESSAGE("Testing Summary UpdateAdditionalInfo on the track");
    auto *pUpdateAlg2 = new Trk::TrackSummaryUpdater("Updater2",pSvcLoc);
    pUpdateAlg2->addRef();
    pUpdateAlg2->setProperty("InputTracksKey","SummaryCreation").ignore();
    pUpdateAlg2->setProperty("OutputTracksKey","AdditionalInfoUpdated").ignore();
    pUpdateAlg2->setProperty("SummaryTool", "SummaryToolStub").ignore();
    //The following means that "updateAdditionalInfo" should be called
    BOOST_TEST(pUpdateAlg2->setProperty("UpdateSharedHits", false).isSuccess());
    BOOST_TEST(pUpdateAlg2->setProperty("UpdateAdditionalInfo", true).isSuccess());
    BOOST_TEST(pUpdateAlg2->initialize().isSuccess());
    BOOST_TEST(pUpdateAlg2->execute().isSuccess());
    keysPresent.clear();
    g.storeGateSvc()->keys<TrackCollection>(keysPresent);
    BOOST_TEST_MESSAGE("StoreGate now contains the following entries: ");
    for (auto & n: keysPresent){
      BOOST_TEST_MESSAGE(n);
    }
    std::string updatedCollectionKey("AdditionalInfoUpdated");
    SG::ReadHandle<TrackCollection> updatedCollection(createdCollectionKey);
    const int updatedValueForNHitsUsed{4};
    for (const auto & aTrack: *updatedCollection){
      const auto & pThisSummary = aTrack->trackSummary();
      const auto valueReadBackForNHitsUsed = pThisSummary->numberOfUsedHitsdEdx();
      BOOST_TEST(valueReadBackForNHitsUsed == updatedValueForNHitsUsed);
    }
    //Update Shared Hits on an existing track summary
    BOOST_TEST_MESSAGE("Testing Summary UpdateSharedHits on the track");
    auto *pUpdateAlg3 = new Trk::TrackSummaryUpdater("Updater3",pSvcLoc);
    pUpdateAlg3->addRef();
    pUpdateAlg3->setProperty("InputTracksKey","SummaryCreation").ignore();
    pUpdateAlg3->setProperty("OutputTracksKey","SharedInfoUpdated").ignore();
    pUpdateAlg3->setProperty("SummaryTool", "SummaryToolStub").ignore();
    //The following means that "UpdateSharedHits" should be called
    BOOST_TEST(pUpdateAlg3->setProperty("UpdateSharedHits", true).isSuccess());
    BOOST_TEST(pUpdateAlg3->setProperty("UpdateAdditionalInfo", false).isSuccess());
    BOOST_TEST(pUpdateAlg3->initialize().isSuccess());
    BOOST_TEST(pUpdateAlg3->execute().isSuccess());
    keysPresent.clear();
    g.storeGateSvc()->keys<TrackCollection>(keysPresent);
    BOOST_TEST_MESSAGE("StoreGate now contains the following entries: ");
    for (auto & n: keysPresent){
      BOOST_TEST_MESSAGE(n);
    }
    std::string updatedSharedCollectionKey("SharedInfoUpdated");
    SG::ReadHandle<TrackCollection> updatedSharedCollection(createdCollectionKey);
    const int updatedSharedHitCount{20};
    for (const auto & aTrack: *updatedCollection){
      const auto & pThisSummary = aTrack->trackSummary();
      const auto valueReadBackSharedHits = pThisSummary->get(Trk::SummaryType::numberOfSCTSharedHits);
      const auto valueReadBackForNHitsUsed = pThisSummary->numberOfUsedHitsdEdx();
      BOOST_TEST(valueReadBackForNHitsUsed == valueSetForNHitsUsed);
      BOOST_TEST(valueReadBackSharedHits == updatedSharedHitCount);
    }
  }
  
BOOST_AUTO_TEST_SUITE_END()


