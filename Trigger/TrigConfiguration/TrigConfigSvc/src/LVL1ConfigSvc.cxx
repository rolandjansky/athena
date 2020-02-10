/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "./LVL1ConfigSvc.h"
#include "./Verifyer.h"


// Athena/Gaudi includes:
#include "PathResolver/PathResolver.h"

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Incident.h"

#include "StoreGate/StoreGateSvc.h"


// Trigger database interface includes:
#include "TrigConfIO/JsonFileLoader.h"
#include "TrigConfIO/TrigDBMenuLoader.h"
#include "TrigConfData/L1Menu.h"

#include "TrigConfL1Data/DeadTime.h"
#include "TrigConfL1Data/CTPConfig.h"
#include "TrigConfL1Data/CTPConfigOnline.h"
#include "TrigConfL1Data/Menu.h"
#include "TrigConfL1Data/ThresholdConfig.h"
#include "TrigConfL1Data/Muctpi.h"
#include "TrigConfL1Data/PrescaledClock.h"
#include "TrigConfL1Data/Random.h"
#include "TrigConfStorage/StorageMgr.h"
#include "TrigConfStorage/XMLStorageMgr.h"
#include "TrigConfL1Data/TriggerThreshold.h"
#include "TrigConfL1Data/TriggerThresholdValue.h"
#include "TrigConfL1Data/ClusterThresholdValue.h"
#include "TrigConfL1Data/EtThresholdValue.h"
#include "TrigConfL1Data/JetThresholdValue.h"
#include "TrigConfL1Data/L1DataDef.h"

#include "TrigConfInterfaces/IJobOptionsSvc.h"

#include "boost/algorithm/string/case_conv.hpp"

// STL includes:
#include <exception>
#include <vector>

using namespace std;
using namespace TrigConf;

LVL1ConfigSvc::LVL1ConfigSvc( const std::string& name, ISvcLocator* pSvcLocator ) :
   base_class( name, pSvcLocator ),
   m_detectorStore(0),
   m_ctpConfig(0),
   m_thrcfg(0),
   m_muctpi(0),
   m_prescaleSetID(0),
   m_bunchgroupSetID(0),
   m_dumpTTVmap(false)
{

   base_class::declareCommonProperties();

   declareProperty( "DBLVL1PSKey", m_prescaleSetID, "L1 Prescale key");
   declareProperty( "DBBGSKey",    m_bunchgroupSetID, "L1 Bunchgroup key");
   declareProperty( "DumpTTVmap",  m_dumpTTVmap, "Dump threshold vector");
}

LVL1ConfigSvc::~LVL1ConfigSvc()
{}


StatusCode
LVL1ConfigSvc::writeConfigToDetectorStore() {

   // do not write empty menu to storegate, so people can check on handlekey being valid
   if( m_inputType == "none" ) {
      ATH_MSG_INFO( "No L1 menu recorded in the detector store" );
      return StatusCode::SUCCESS;
   }

   TrigConf::L1Menu * l1menu = new TrigConf::L1Menu();

   if( m_inputType == "db" ) {

      // db menu loader
      TrigConf::TrigDBMenuLoader dbloader(m_dbConnection);
      dbloader.setLevel(TrigConf::MSGTC::WARNING);

      if( dbloader.loadL1Menu( m_smk, *l1menu ) ) {
         ATH_MSG_INFO( "Loaded L1 menu from DB " << m_dbConnection << " for SMK " << m_smk.value() );
      } else {
         ATH_MSG_WARNING( "Failed loading L1 menu from DB for SMK " << m_smk.value());
         return StatusCode::RECOVERABLE;
      }

   } else if ( m_inputType == "file" ) {

      // json file menu loader
      TrigConf::JsonFileLoader fileLoader;
      fileLoader.setLevel(TrigConf::MSGTC::WARNING);

      if( fileLoader.loadFile( m_l1FileName, *l1menu ) ) {
         ATH_MSG_INFO( "Loaded L1 menu file " << m_l1FileName.value() );
      } else {
         ATH_MSG_WARNING( "Failed loading L1 menu file " << m_l1FileName.value());
         return StatusCode::RECOVERABLE;
      }

   } else if( m_inputType == "cool" ) {
      ATH_MSG_FATAL( "Loading of L1 menu from COOL + DB not implemented");
      return StatusCode::FAILURE;
   }

   // auto writeHandle = SG::makeHandle(m_l1MenuKey);
   // ATH_MSG_INFO("Recording L1 menu with " << m_l1MenuKey);
   // ATH_CHECK( writeHandle.record( std::unique_ptr<TrigConf::L1Menu>(l1menu) ));

   ServiceHandle<StoreGateSvc> detStore( "StoreGateSvc/DetectorStore", name() );   
   ATH_CHECK( detStore.retrieve() );
   if( detStore->record(l1menu,"L1TriggerMenu").isSuccess() ) {
      ATH_MSG_INFO( "Recorded L1 menu with key 'L1TriggerMenu' in the detector store" );
   }

   return StatusCode::SUCCESS;
} 



StatusCode
LVL1ConfigSvc::initialize() {

   /// Handle to JobOptionsSvc used to retrieve the DataFlowConfig property

   // ATH_CHECK( m_l1MenuKey.initialize() );

   if( auto joSvc = serviceLocator()->service<TrigConf::IJobOptionsSvc>( "JobOptionsSvc" ) ) {
      if( joSvc->superMasterKey()>0 ) {
         m_inputType = "db";
         m_smk = joSvc->superMasterKey();
         m_dbConnection = joSvc->server();
         m_configSourceString = "NONE";
      }
   } else {
      ATH_MSG_INFO("Did not locate TrigConf::JobOptionsSvc, not running athenaHLT");
   }

   StatusCode sc = writeConfigToDetectorStore();
   if( !sc.isSuccess() ) {
          ATH_MSG_INFO( "This previous WARNING message is being ignored in the current transition phase. Once we rely entirely on the new menu providing mechanism, this will become a reason to abort.");
   }

   CHECK(ConfigSvcBase::initialize());
   
   // could be that this is needed when running from DB
   CHECK(setProperties());

   if( !fromDB() and m_xmlFile=="NONE" ) {
      ATH_MSG_INFO("xml file set to NONE, will not load LVL1 Menu");
      return StatusCode::SUCCESS;
   }

   if( fromDB() ) {
      ATH_MSG_INFO("  DB LVL1 PrescaleKey  = " << m_prescaleSetID);
      ATH_MSG_INFO("  DB BunchGroupSetKey  = " << m_bunchgroupSetID);
   }

   CHECK( initStorageMgr() );

   try {

      //       m_storageMgr->setLevel(MSGTC::DEBUG);  // TODO check if this is propagated to the loader
      //       m_storageMgr->masterTableLoader().setLevel(MSGTC::DEBUG);

      m_ctpConfig = new CTPConfig();
      m_ctpConfig->setSMK( m_dbSMKey );
      m_ctpConfig->setPrescaleSetId( m_prescaleSetID );
      m_ctpConfig->setBunchGroupSetId( m_bunchgroupSetID );
      if(!m_storageMgr->masterTableLoader().load( *m_ctpConfig ))
      {
         ATH_MSG_ERROR("Could not load CTPConfig object");
         return StatusCode::FAILURE;
      }
      ATH_MSG_VERBOSE("CTPConfig object loaded");

      m_thrcfg = &m_ctpConfig->menu().thresholdConfig();

      m_muctpi = new Muctpi();
      m_muctpi->setSMK( m_dbSMKey );
      //m_muctpi->setLvl1MasterTableId( 0 );
      m_storageMgr->masterTableLoader().load( *m_muctpi );
      ATH_MSG_VERBOSE("MuCTPI configuration object loaded");

   } catch ( std::exception & e ) {

      CHECK(freeStorageMgr());

      ATH_MSG_ERROR("Standard C++ exception caught: " << e.what());
      return StatusCode::FAILURE;

   } catch ( ... ) {

      CHECK(freeStorageMgr());

      ATH_MSG_ERROR("Unknown exception caught.");
      return StatusCode::FAILURE;

   }

   CHECK(freeStorageMgr());


   //
   // Check if we have a valid trigger menu:
   //
   if( Verifyer::verify( m_ctpConfig->menu(), msg() ) ) {
      ATH_MSG_ERROR("Loaded trigger menu fails verification");
      return StatusCode::FAILURE;
   } else {
      ATH_MSG_DEBUG("Loaded trigger menu passed verification");
   }

   ATH_MSG_DEBUG("Dumping LVL1 configuration");

   if (msgSvc()->outputLevel(name()) <= MSG::INFO) {
      m_ctpConfig->print("  ", m_printMenuLevel);
   }

   // Dump threshold value map
   if (m_dumpTTVmap) {
      ATH_MSG_DEBUG("Dumping ThresholdValue map from ThresholdConfig");
      m_thrcfg->printThresholdValueMap();
   }

   // Get a hold of the DetectorStore:
   CHECK(service( "DetectorStore", m_detectorStore ));

   //
   // fire incident
   // 
   ServiceHandle<IIncidentSvc> incSvc("IncidentSvc",name());
   if (incSvc.retrieve().isFailure()) {
      ATH_MSG_WARNING("Cannot retrieve IncidentSvc");
   } else {
      // prefered
      std::string incname = "";
      incname += "L1Menu";
      incname += "L1PS";
      incSvc->fireIncident(Incident(incname,"TrigConf"));
      ATH_MSG_INFO("Fired Incident 'TrigConf' - " << incname);    
   }

   return StatusCode::SUCCESS;
}

StatusCode
LVL1ConfigSvc::finalize() {

   ATH_MSG_DEBUG("Finalizing");

   // Delete configuration objects:
   //delete m_thrcfg;
   delete m_ctpConfig;
   delete m_muctpi;

   CHECK(AthService::finalize());

   return StatusCode::SUCCESS;

}

StatusCode
LVL1ConfigSvc::queryInterface( const InterfaceID& riid, void** ppvIF ) {

   StatusCode sc = StatusCode::FAILURE;

   if( ppvIF ) {
      *ppvIF = 0;

      if( riid == ILVL1ConfigSvc::interfaceID() ) {
         try {
            *ppvIF = dynamic_cast<ILVL1ConfigSvc*>( this );
         } catch( const bad_cast& ) {
            return StatusCode::FAILURE;
         }
         sc = StatusCode::SUCCESS;
      } else {
         sc = Service::queryInterface( riid, ppvIF );
      }

   }

   return sc;

}


const BunchGroupSet*
LVL1ConfigSvc::bunchGroupSet() const { 
   if (ctpConfig()==0) return 0;
   return &ctpConfig()->bunchGroupSet();
}
