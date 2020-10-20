/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
/**
 * @file TrackCollectionMerger/test/TrackCollectionMerger_test.cxx
 * @author Shaun Roe
 * @date Feb, 2019
 * @brief Some tests for TrackCollectionMerger algorithm in the Boost framework
 */
 
 
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE TEST_TRKTRACKCOLLECTIONMERGER
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
#include "CxxUtils/ubsan_suppress.h"
#include "PutTrackCollectionsInSG.h"

#include "StoreGate/ReadHandleKey.h"
#include "TrkTrack/TrackCollection.h"
#include "../TrkTrackCollectionMerger/TrackCollectionMerger.h"
#include <vector>

ATLAS_NO_CHECK_FILE_THREAD_SAFETY; // This test uses global svcLoc.


// Gaudi fixture
class GaudiFixture {
 public:
   ISvcLocator * 
   svcLoc(){
     return m_svcLoc;
   }
   
   StoreGateSvc* 
   storeGateSvc(){
    return m_sg;
   }

   GaudiFixture(const std::string & joPath = "TrkTrackCollectionMerger/TrackCollectionMerger_test.txt") {
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
     //"TrkTrackCollectionMerger/TrackCollectionMerger_test.txt"
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


BOOST_AUTO_TEST_SUITE(TrackCollectionMergerTest)
  GaudiFixture g("TrkTrackCollectionMerger/TrackCollectionMerger_test.txt");
  auto  pSvcLoc=g.svcLoc();

  BOOST_AUTO_TEST_CASE( sanityCheck ){
    const bool svcLocatorIsOk=(pSvcLoc != nullptr);
    BOOST_TEST(svcLocatorIsOk);
  }
  
  BOOST_AUTO_TEST_CASE(Execute){
    auto pAlg = new PutTrackCollectionsInSG("PutCollectionsInSG",pSvcLoc);
    pAlg->addRef();
    BOOST_TEST(pAlg->execute().isSuccess());
    std::string collectionKey1("StoreGateSvc+TrackCollectionKey1");
    SG::ReadHandle<TrackCollection> thisTrackCollection1 (collectionKey1);
    BOOST_TEST(thisTrackCollection1->size() == 1);
    std::string collectionKey2("StoreGateSvc+TrackCollectionKey2");
    SG::ReadHandle<TrackCollection> thisTrackCollection2 (collectionKey2);
    BOOST_TEST(thisTrackCollection2->size() == 1);
    auto pMergeAlg = new Trk::TrackCollectionMerger("TrackCollectionMerger",pSvcLoc);
    pMergeAlg->addRef();
    BOOST_TEST(pMergeAlg->setProperty("TracksLocation","['TrackCollectionKey1','TrackCollectionKey2']").isSuccess());
    //initialize() is necessary here
    BOOST_TEST(pMergeAlg->initialize().isSuccess());
    BOOST_TEST(pMergeAlg->execute().isSuccess());
    std::vector<std::string> keysPresent{};
    g.storeGateSvc()->keys<TrackCollection>(keysPresent);
    for (auto & n: keysPresent){
      BOOST_TEST_MESSAGE(n);
    }
    std::string mergedCollectionKey("CombinedInDetTracks");
    SG::ReadHandle<TrackCollection> mergedCollection (mergedCollectionKey);
    BOOST_TEST(mergedCollection->size() == 2);
    //
    
    const std::vector<std::string> expectedKeys{"CombinedInDetTracks","TrackCollectionKey1", "TrackCollectionKey2"};
    g.storeGateSvc()->keys<TrackCollection>(keysPresent);
    BOOST_TEST(keysPresent == expectedKeys, boost::test_tools::per_element());
  }
  
BOOST_AUTO_TEST_SUITE_END()


