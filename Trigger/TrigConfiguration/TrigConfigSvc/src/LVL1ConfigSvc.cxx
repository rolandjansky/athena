/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "./LVL1ConfigSvc.h"
#include "./Verifyer.h"


// Athena/Gaudi includes:
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

#include "TrigConfBase/TrigDBConnectionConfig.h"

#include "TrigConfInterfaces/IJobOptionsSvc.h"

#include "boost/algorithm/string.hpp"

// STL includes:
#include <exception>
#include <vector>

using namespace std;

TrigConf::LVL1ConfigSvc::LVL1ConfigSvc( const std::string& name, ISvcLocator* pSvcLocator ) :
   base_class( name, pSvcLocator )
{
   base_class::declareCommonProperties();
}

TrigConf::LVL1ConfigSvc::~LVL1ConfigSvc()
{}

const TrigConf::ThresholdConfig*
TrigConf::LVL1ConfigSvc::thresholdConfig() const { return m_ctpConfig ? &m_ctpConfig->menu().thresholdConfig() : nullptr; }


StatusCode
TrigConf::LVL1ConfigSvc::initializeRun3StyleMenu() {

   m_inputType = boost::to_lower_copy(m_inputType.value()); // lower case

   if( auto joSvc = serviceLocator()->service<TrigConf::IJobOptionsSvc>( "JobOptionsSvc" ) ) {
      if( joSvc->superMasterKey()>0 ) {
         ATH_MSG_INFO("Taking new style menu from db with setting from TrigConf::JobOptionsSvc");
         m_inputType = "db";
         m_smk = joSvc->superMasterKey();
         m_dbConnection = joSvc->server();
      }
   }

   ATH_MSG_INFO("Run 3 style menu configuration");
   ATH_MSG_INFO("    Run 3 input type  = " << m_inputType.value());
   if( m_inputType == "file" ) {
      ATH_MSG_INFO("    Run 3 input file  = " << m_l1FileName.value());
   } else if ( m_inputType == "db" ) {
      ATH_MSG_INFO("    Run 3 DB connection  = " << m_dbConnection);
      ATH_MSG_INFO("    Run 3 SMK            = " << m_smk);
   }
   if( ! loadRun3StyleMenu().isSuccess() ) {
      ATH_MSG_INFO( "The previous WARNING message is being ignored in the current transition phase. Once we rely entirely on the new menu providing mechanism, this will become a reason to abort.");
   }
   return StatusCode::SUCCESS;
}


StatusCode
TrigConf::LVL1ConfigSvc::loadRun3StyleMenu() {
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
TrigConf::LVL1ConfigSvc::initializeRun2StyleMenu() {

   m_configSourceString = boost::to_lower_copy(m_configSourceString); // lower case

   if ( m_configSourceString == "none" ) {
      ATH_MSG_INFO("Run 2 style menu has been disabled");
      m_xmlFile = "";
   } else if( m_configSourceString != "xml") {
      TrigDBConnectionConfig::DBType dbtype(TrigDBConnectionConfig::DBLookup);
      if (m_configSourceString == "oracle") { dbtype = TrigDBConnectionConfig::Oracle; }
      else if (m_configSourceString == "mysql")  { dbtype = TrigDBConnectionConfig::MySQL; }
      else if (m_configSourceString == "sqlite") { dbtype = TrigDBConnectionConfig::SQLite; }
      if(m_dbHLTPSKey) {
         m_dbconfig = std::make_unique<TrigDBConnectionConfig>( dbtype, m_dbServer, m_dbSMKey, m_dbHLTPSKey );
      } else {
         m_dbconfig = std::make_unique<TrigDBConnectionConfig>( dbtype, m_dbServer, m_dbSMKey, m_dbHLTPSKeySet );
      }
      m_dbconfig->m_useFrontier = m_useFrontier;
   }

   ATH_MSG_INFO("Run 2 style menu");
   ATH_MSG_INFO("    Run 2 config source  = " << m_configSourceString);
   if( m_configSourceString != "none" ) {
      if( m_configSourceString == "xml" ) {
         ATH_MSG_INFO("    Run 2 Input file     = " << m_xmlFile);
      } else {
         ATH_MSG_INFO("    Run 2 DB Server      = " << m_dbServer);
         ATH_MSG_INFO("    Run 2 DB User        = " << m_dbUser);
         ATH_MSG_INFO("    Run 2 DB Table       = " << m_dbTable);
         ATH_MSG_INFO("    Run 2 DB connection  = " << m_dbconfig->toString());
         ATH_MSG_INFO("    Run 2 SMK            = " << m_dbSMKey);
         ATH_MSG_INFO("    Run 2 L1 PSK         = " << m_prescaleSetID);
         ATH_MSG_INFO("    Run 2 BGSK           = " << m_bunchgroupSetID);
         ATH_MSG_INFO("    UseFrontier          = " << m_useFrontier.value());
      }
   }

   if( m_dbconfig==nullptr && m_xmlFile=="NONE" ) {
      ATH_MSG_INFO("xml file set to NONE, will not load LVL1 Menu");
   } else {
      CHECK( loadRun2StyleMenu() );
   }
   return StatusCode::SUCCESS;
}


/**
 * Loading Run-2-style menu
 */
StatusCode
TrigConf::LVL1ConfigSvc::loadRun2StyleMenu() {
   CHECK( initStorageMgr() );
   try {
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
      m_muctpi = new Muctpi();
      m_muctpi->setSMK( m_dbSMKey );
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

   /**
    * Check if we have a valid trigger menu:
    */
   if( Verifyer::verify( m_ctpConfig->menu(), msg() ) ) {
      ATH_MSG_ERROR("Loaded trigger menu fails verification");
      return StatusCode::FAILURE;
   } else {
      ATH_MSG_DEBUG("Loaded trigger menu passed verification");
   }

   return StatusCode::SUCCESS;
}


StatusCode
TrigConf::LVL1ConfigSvc::initialize() {

   CHECK(AthService::initialize());

   /// Handle to JobOptionsSvc used to retrieve the DataFlowConfig property
   ATH_MSG_INFO("=================================");
   ATH_MSG_INFO("Initializing " << name() << " service");
   ATH_MSG_INFO("=================================");

   /*
     initializing and load new and old menu
   */
   CHECK( initializeRun3StyleMenu() );

   CHECK( initializeRun2StyleMenu() );

   //
   // fire incident (this has to be revisited)
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
TrigConf::LVL1ConfigSvc::finalize() {

   ATH_MSG_DEBUG("Finalizing");

   delete m_ctpConfig;
   delete m_muctpi;

   CHECK(AthService::finalize());

   return StatusCode::SUCCESS;

}

StatusCode
TrigConf::LVL1ConfigSvc::queryInterface( const InterfaceID& riid, void** ppvIF ) {

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


const TrigConf::BunchGroupSet*
TrigConf::LVL1ConfigSvc::bunchGroupSet() const { 
   if (ctpConfig()==0) return 0;
   return &ctpConfig()->bunchGroupSet();
}
