/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfStorage/StorageMgr.h"
#include "TrigConfStorage/SessionMgr.h"

#include "./MasterTableLoader.h"
#include "./MenuLoader.h"
#include "./MuctpiLoader.h"
#include "./DeadTimeLoader.h"
#include "./PrescaledClockLoader.h"
#include "./RandomLoader.h"
#include "./ThresholdConfigLoader.h"
#include "./LutCamLoader.h"
#include "./ThresholdMonitorLoader.h"
#include "./CTPFilesLoader.h"
#include "./PrescaleSetLoader.h"
#include "./PrioritySetLoader.h"
#include "./BunchGroupLoader.h"
#include "./BunchGroupSetLoader.h"
#include "./CTPConfigLoader.h"
#include "./CTPConfigOnlineLoader.h"
#include "./TriggerThresholdLoader.h"
#include "./TriggerThresholdValueLoader.h"
#include "./CaloJetInputLoader.h"
#include "./CaloSinCosLoader.h"
#include "./CaloInfoLoader.h"
#include "./MuonThresholdSetLoader.h"
#include "./HLTFrameLoader.h"
#include "./HLTPrescaleSetLoader.h"
#include "./HLTPrescaleSetCollectionLoader.h"
#include "./JobOptionTableLoader.h"
#include "./L1TopoMenuLoader.h"

#include <iostream>
#include <memory>

using namespace std;
using namespace TrigConf;

TrigConf::StorageMgr::StorageMgr( const std::string& cs,
                                  const std::string& user,
                                  const std::string& pass,
                                  std::ostream & /*o*/) :
   TrigConfMessaging("DBStorageMgr")
   , m_cs( cs )
   , m_user( user )
   , m_password( pass )
{}

TrigConf::StorageMgr::StorageMgr( const std::string& type,
                                  const std::string& server,
                                  const std::string& name,
                                  const std::string& user,
                                  const std::string& pass,
                                  std::ostream & /*o*/) :
   TrigConfMessaging("DBStorageMgr")
   , m_dbtype( type )
   , m_dbserver( server )
   , m_dbname( name )    
   , m_user( user )
   , m_password( pass )
{}


StorageMgr::~StorageMgr() {
   delete m_hltPrescaleSetLoader;
   delete m_hltPrescaleSetCollectionLoader;
   delete m_sessionMgr;
}


// setting log level of all loaders and itself
void
StorageMgr::setLevel(MSGTC::Level lvl) {
   msg().setLevel(lvl);
}


void
StorageMgr::closeSession() {
   this->sessionMgr().closeSession();
}


SessionMgr&
StorageMgr::sessionMgr() {
   if( m_sessionMgr == 0 ) {
      m_sessionMgr = new SessionMgr();
      if( m_cs != "" ) {
         m_sessionMgr->setConnectionString(m_cs);
      } else {
         m_sessionMgr->setDbType(m_dbtype);
         m_sessionMgr->setDbServer(m_dbserver);
         m_sessionMgr->setDbName(m_dbname);
         m_sessionMgr->setDbUser(m_user);
         m_sessionMgr->setDbPassword(m_password);
      }
      m_sessionMgr->setUseFrontier(useFrontier()); 
      m_sessionMgr->setRetrialPeriod(m_retrialPeriod);
      m_sessionMgr->setRetrialTimeout(m_retrialTimeout);
      m_sessionMgr->setConnectionTimeout(m_connectionTimeout);
   }
   return * m_sessionMgr;
}



IL1TopoMenuLoader& StorageMgr::l1topoMenuLoader() {
   if (!m_l1topoMenuLoader) m_l1topoMenuLoader = NewLoader<L1TopoMenuLoader>();
   return *m_l1topoMenuLoader;
}
  
TrigConf::IMasterTableLoader& TrigConf::StorageMgr::masterTableLoader() {
   if (!m_masterTableLoader) m_masterTableLoader = NewLoader<MasterTableLoader>();
   return *m_masterTableLoader;
}
  
TrigConf::IMenuLoader& TrigConf::StorageMgr::menuLoader() {
   if( ! m_menuLoader ) m_menuLoader = NewLoader<MenuLoader>();
   return *m_menuLoader;
}

TrigConf::IMuctpiLoader& TrigConf::StorageMgr::muctpiLoader() {
   if( ! m_muctpiLoader ) m_muctpiLoader = NewLoader<MuctpiLoader>();
   return *m_muctpiLoader;
}

TrigConf::IDeadTimeLoader& TrigConf::StorageMgr::deadTimeLoader() {
   if( ! m_deadTimeLoader ) m_deadTimeLoader = NewLoader<DeadTimeLoader>();
   return *m_deadTimeLoader;
}

TrigConf::IPrescaledClockLoader& TrigConf::StorageMgr::prescaledClockLoader() {
   if( ! m_prescaledClockLoader ) m_prescaledClockLoader = NewLoader<PrescaledClockLoader>();
   return *m_prescaledClockLoader;
}

TrigConf::IRandomLoader& TrigConf::StorageMgr::randomLoader() {
   if( ! m_randomLoader ) m_randomLoader = NewLoader<RandomLoader>();
   return *m_randomLoader;
}

TrigConf::IThresholdConfigLoader& TrigConf::StorageMgr::thresholdConfigLoader() {
   if( ! m_thresholdConfigLoader ) m_thresholdConfigLoader = NewLoader<ThresholdConfigLoader>();
   return *m_thresholdConfigLoader;
}

TrigConf::ITriggerThresholdLoader& TrigConf::StorageMgr::triggerThresholdLoader() {
   if( ! m_triggerThresholdLoader ) m_triggerThresholdLoader = NewLoader<TriggerThresholdLoader>();
   return *m_triggerThresholdLoader;
}

TrigConf::ITriggerThresholdValueLoader& TrigConf::StorageMgr::triggerThresholdValueLoader() {
   if( ! m_triggerThresholdValueLoader ) m_triggerThresholdValueLoader = NewLoader<TriggerThresholdValueLoader>();
   return *m_triggerThresholdValueLoader;
}

TrigConf::ICaloJetInputLoader& TrigConf::StorageMgr::caloJetInputLoader() {
   if( ! m_caloJetInputLoader ) m_caloJetInputLoader = NewLoader<CaloJetInputLoader>();
   return *m_caloJetInputLoader;
}

TrigConf::ICaloSinCosLoader& TrigConf::StorageMgr::caloSinCosLoader() {
   if( ! m_caloSinCosLoader ) m_caloSinCosLoader = NewLoader<CaloSinCosLoader>();
   return *m_caloSinCosLoader;
}

TrigConf::ICaloInfoLoader& TrigConf::StorageMgr::caloInfoLoader() {
   if( ! m_caloInfoLoader ) m_caloInfoLoader = NewLoader<CaloInfoLoader>();
   return *m_caloInfoLoader;
}

TrigConf::IThresholdMonitorLoader& TrigConf::StorageMgr::thresholdMonitorLoader() {
   if( ! m_thresholdMonitorLoader ) m_thresholdMonitorLoader = NewLoader<ThresholdMonitorLoader>();
   return *m_thresholdMonitorLoader;
}

TrigConf::ICTPFilesLoader& TrigConf::StorageMgr::ctpFilesLoader() 	 
{ 	 
   if( ! m_ctpFilesLoader ) m_ctpFilesLoader = NewLoader<CTPFilesLoader>(); 	 
   return *m_ctpFilesLoader; 	 
}

TrigConf::ILutCamLoader& TrigConf::StorageMgr::lutcamLoader() {
   if( ! m_lutCamLoader ) m_lutCamLoader = NewLoader<LutCamLoader>();
   return *m_lutCamLoader;
}

TrigConf::IPrescaleSetLoader& TrigConf::StorageMgr::prescaleSetLoader() {
   if( ! m_prescaleSetLoader ) m_prescaleSetLoader = NewLoader<PrescaleSetLoader>();
   return *m_prescaleSetLoader;
}

TrigConf::IPrioritySetLoader& TrigConf::StorageMgr::prioritySetLoader() {
   if( ! m_prioritySetLoader ) m_prioritySetLoader = NewLoader<PrioritySetLoader>();
   return *m_prioritySetLoader;
}

TrigConf::IBunchGroupLoader& TrigConf::StorageMgr::bunchGroupLoader() {
   if( ! m_bunchGroupLoader ) m_bunchGroupLoader = NewLoader<BunchGroupLoader>();
   return *m_bunchGroupLoader;
}

TrigConf::IBunchGroupSetLoader& TrigConf::StorageMgr::bunchGroupSetLoader() {
   if( ! m_bunchGroupSetLoader ) m_bunchGroupSetLoader = NewLoader<BunchGroupSetLoader>();
   return *m_bunchGroupSetLoader;
}

TrigConf::ICTPConfigLoader& TrigConf::StorageMgr::ctpConfigLoader() {
   if( ! m_ctpConfigLoader ) m_ctpConfigLoader = NewLoader<CTPConfigLoader>();
   return *m_ctpConfigLoader;
}

TrigConf::ICTPConfigLoader& TrigConf::StorageMgr::ctpConfigOnlineLoader() {
   return ctpConfigLoader();
}

TrigConf::IMuonThresholdSetLoader& TrigConf::StorageMgr::muonThresholdSetLoader() {
   if( ! m_muonThresholdSetLoader ) m_muonThresholdSetLoader = NewLoader<MuonThresholdSetLoader>();
   return *m_muonThresholdSetLoader;
}

TrigConf::IHLTFrameLoader& TrigConf::StorageMgr::hltFrameLoader() {
   if( ! m_HLTFrameLoader ) m_HLTFrameLoader = NewLoader<HLTFrameLoader>();
   return *m_HLTFrameLoader;
}

TrigConf::IJobOptionTableLoader& TrigConf::StorageMgr::jobOptionTableLoader() {
   if( ! m_jobOptionTableLoader ) m_jobOptionTableLoader = NewLoader<JobOptionTableLoader>();
   return *m_jobOptionTableLoader;
}

TrigConf::IHLTPrescaleSetLoader& TrigConf::StorageMgr::hltPrescaleSetLoader() {
   if( ! m_hltPrescaleSetLoader ) m_hltPrescaleSetLoader = NewLoader<HLTPrescaleSetLoader>();
   return *m_hltPrescaleSetLoader;
}

TrigConf::IHLTPrescaleSetCollectionLoader& TrigConf::StorageMgr::hltPrescaleSetCollectionLoader() {
   if( ! m_hltPrescaleSetCollectionLoader ) m_hltPrescaleSetCollectionLoader = NewLoader<HLTPrescaleSetCollectionLoader>();
   return *m_hltPrescaleSetCollectionLoader;
}
