/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// STL includes:
#include <exception>
#include <vector>

// Athena/Gaudi includes:
#include "PathResolver/PathResolver.h"

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Incident.h"

#include "StoreGate/StoreGateSvc.h"

// Local includes:
#include "TrigConfigSvc/Verifyer.h"

// Trigger database interface includes:

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

#include "TrigConfigSvc/LVL1ConfigSvc.h"

#include "boost/algorithm/string/case_conv.hpp"
#include "boost/lexical_cast.hpp"

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
LVL1ConfigSvc::initialize() {

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

   CHECK(initStorageMgr());

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
   ATH_MSG_INFO("Disabled L1 menu verification for Run-3 studies");
//    if( Verifyer::verify( m_ctpConfig->menu(), msg() ) ) {
//       ATH_MSG_ERROR("Loaded trigger menu fails verification");
//       return StatusCode::FAILURE;
//    } else {
//       ATH_MSG_DEBUG("Loaded trigger menu passed verification");
//    }

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
         } catch( bad_cast ) {
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
