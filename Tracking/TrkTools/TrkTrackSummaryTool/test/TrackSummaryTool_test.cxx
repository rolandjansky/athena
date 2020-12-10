/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
/**
 * @file TrkTrackSummaryUpdater/test/TrackSummaryUpdater_test.cxx
 * @author Shaun Roe
 * @date Dec, 2020
 * @brief Some tests for TrackSummaryTool algorithm in the Boost framework
 */
 
 
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE TEST_TRKTRACKSUMMARYTOOL
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
#include "GaudiKernel/EventContext.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "CxxUtils/ubsan_suppress.h"
#include "IdDictParser/IdDictParser.h"
#include "AtlasDetDescr/AtlasDetectorID.h"

#include "TrkTrackSummaryTool/TrackSummaryTool.h"
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
   StoreGateSvc* 
   detStore(){
    return m_detStore;
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
     m_svcLoc->service ("StoreGateSvc/DetectorStore", m_detStore).ignore();
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
   StoreGateSvc *           m_detStore{nullptr};
 };

BOOST_AUTO_TEST_SUITE(TrackSummaryUpdaterTest)
 
  GaudiFixture g("TrkTrackSummaryTool/TrackSummaryTool_test.txt");
  auto  pSvcLoc=g.svcLoc();
  auto  pToolSvc=g.toolSvc();
  auto  pDetStore=g.detStore();
  IAlgTool* pToolInterface{};
  
  BOOST_AUTO_TEST_CASE( sanityCheck ){
    const bool svcLocatorIsOk=(pSvcLoc != nullptr);
    BOOST_TEST(svcLocatorIsOk);
    const bool toolSvcIsOk = ( pToolSvc != nullptr);
    BOOST_TEST(toolSvcIsOk);
    const bool detStoreIsOk = (pDetStore != nullptr);
    BOOST_TEST(detStoreIsOk);
  }
  
  BOOST_AUTO_TEST_CASE(retrieveTool){
    static IdDictParser parser;
    parser.register_external_entity ("InnerDetector","IdDictInnerDetector.xml");
    parser.register_external_entity ("MuonSpectrometer","IdDictMuonSpectrometer_S.02.xml");
    parser.register_external_entity ("Calorimeter","IdDictCalorimeter_L1Onl.xml");
        IdDictMgr& idDict = parser.parse ("IdDictParser/ATLAS_IDS.xml");
    auto atlasId = std::make_unique<AtlasDetectorID>();
    atlasId->initialize_from_dictionary (idDict);
    if (pDetStore and (not pDetStore->contains<AtlasDetectorID>("AtlasID"))) {
      BOOST_TEST ( pDetStore->record (std::move (atlasId), "AtlasID").isSuccess() );
    }
    BOOST_TEST ( pToolSvc->retrieveTool("Trk::TrackSummaryTool", pToolInterface).isSuccess());
    BOOST_TEST(pToolInterface -> initialize());
  }
  

BOOST_AUTO_TEST_SUITE_END()


