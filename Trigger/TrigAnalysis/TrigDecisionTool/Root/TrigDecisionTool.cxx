/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: TrigDecisionTool
 * @Package: TrigDecisionTool
 * @Class  : TrigDecisionTool
 *
 * @brief main tool
 *
 * @author Michael Begel  <michael.begel@cern.ch> - Brookhaven National Laboratory
 * @author Lukas Heinrich  <lukas.heinrich@cern.ch> - NYU
 *
 ***********************************************************************************/

#include "TrigDecisionTool/DecisionUnpackerAthena.h"
#include "TrigDecisionTool/DecisionUnpackerStandalone.h"
#include "TrigDecisionTool/TrigDecisionTool.h"

#include "TrigConfHLTData/HLTChainList.h"
#include "TrigConfL1Data/CTPConfig.h"
#include "TrigConfL1Data/Menu.h"

/// Number of TDT instances
static std::atomic<int> s_instances = 0;


Trig::TrigDecisionTool::TrigDecisionTool(const std::string& name)
  : Logger(this)
  , asg::AsgMetadataTool(name)
{
#ifdef XAOD_ANALYSIS // AthAnalysis or AnalysisBase
  m_navigation = &m_standaloneNavigation;
#else  //full Athena env
  m_navigation = &*m_fullNavigation;
#endif
}


StatusCode
Trig::TrigDecisionTool::initialize() {
   TrigDecisionToolCore::initialize().ignore();

   if (m_navigationFormat != "TriggerElement" && m_navigationFormat != "TrigComposite") {
     ATH_MSG_ERROR("NavigationFormat property must be one of 'TriggerElement' for Run 1, 2 triggered input or 'TrigComposite' for Run 3+ triggered input");
     return StatusCode::FAILURE;
   }


#ifndef XAOD_ANALYSIS // Full Athena only
   ATH_CHECK(m_oldDecisionKey.initialize( m_useRun1DecisionFormat ) );
   ATH_CHECK(m_oldEventInfoKey.initialize( m_useOldEventInfoDecisionFormat ) );
#endif

   ATH_CHECK(m_HLTSummaryKeyIn.initialize(m_navigationFormat == "TrigComposite"));
   ATH_CHECK(m_navigationKey.initialize(m_navigationFormat == "TriggerElement"));
   ATH_CHECK(m_decisionKey.initialize());

   ++s_instances;
   if ( s_instances > 1) {
     ATH_MSG_WARNING("Multiple TrigDecisionTool instances created. "
                     "This is not efficent from performance perspective. "
                     "Access of the same EDM objects will give warnings." );
     if (!m_acceptMultipleInstance){
       ATH_MSG_ERROR("Will not accept multiple instances. "
                     "Set 'AcceptMultipleInstance' to overwrite this behavior.");
       return StatusCode::FAILURE;
     }
   }

   ATH_MSG_INFO("Initializing Trig::TrigDecisionTool (standalone version even for athena)");
  
#ifndef XAOD_STANDALONE // AthAnalysis or full Athena
   // We setup the TrigConfigSvc interface
   // This is the standalone xAOD ConfigSvc
   // In Run 3 the standalone xAOD ConfigSvc is useable in both RAWtoALL jobs and from ESD, AOD.
   
   if(m_configSvc.empty()) {
     ATH_MSG_DEBUG("No TrigConfigSvc provided. Using ConfigTool instead...");
     CHECK( m_configTool.retrieve()); //use configTool if no configSvc available
   } else {

     m_configTool.disable();

     ATH_MSG_DEBUG("Fetching " << m_configSvc.typeAndName());
     ATH_CHECK(m_configSvc.retrieve());

     // call update if there is anything in config svc
     if ( m_configSvc->ctpConfig() ) {
       configurationUpdate( &m_configSvc->chains(), m_configSvc->ctpConfig() );
     }
   }
#else // AnalysisBase
   ATH_CHECK(m_configTool.retrieve());
#endif

#ifndef XAOD_ANALYSIS // Full athena only
   if (m_navigationFormat == "TriggerElement") {
     ATH_CHECK(m_fullNavigation.retrieve());
   }
#endif

   cgm()->navigation(&*m_navigation);

   cgm()->setStore(&*evtStore()); // Cache must be updated per-event too (when running with multiple slots)
   
#ifndef XAOD_STANDALONE // AthAnalysis or full Athena
   ServiceHandle<IIncidentSvc> incSvc("IncidentSvc",name());
   if (incSvc.retrieve().isFailure()) {
     ATH_MSG_ERROR("Cannot retrieve IncidentSvc");
     return StatusCode::FAILURE;
   }

   long int pri=-1; //ensure happens *after* TrigConfxAOD tool ... use -1 priority
   incSvc->addListener( this, "TrigConf", pri );
   incSvc->removeListener( this, "BeginEvent");  //beginEvent is added by AsgMetadataTool already! ...DONT ADD IT AGAIN ... especially at priority 100!
   incSvc->addListener( this, "BeginEvent", pri );
   //do the same adjustment for the BeginInputFile incident, just to be safe 
   incSvc->removeListener( this, IncidentType::BeginInputFile );
   incSvc->addListener( this, IncidentType::BeginInputFile, pri, true/*rethrow*/);
#endif

   ATH_MSG_INFO("Initialized TDT" );

   std::map<std::string, std::string>::const_iterator pIt;
   for ( pIt = m_publicChainGroups.begin(); pIt != m_publicChainGroups.end(); ++pIt ) {
      std::vector<std::string> patterns;
      patterns.push_back(pIt->second);
      cgm()->createChainGroup(patterns, pIt->first);
      ATH_MSG_INFO("created Public Chain Group " << pIt->first << " with pattern: " << pIt->second );
   }
 
   return StatusCode::SUCCESS;
}

std::vector<uint32_t>* Trig::TrigDecisionTool::getKeys() {
#ifndef XAOD_STANDALONE // AthAnalysis or full Athena
  return m_configKeysCache.get();
#else // AnalysisBase
  return &m_configKeysCache;
#endif 
}

void Trig::TrigDecisionTool::setForceConfigUpdate(bool b, bool forceForAllSlots) {
#ifndef XAOD_STANDALONE // AthAnalysis or full Athena
  {
    std::atomic<bool>* ab = m_forceConfigUpdate.get();
    (*ab) = b;
  }
  if (forceForAllSlots) {
    for (size_t dummySlot = 0; dummySlot < SG::getNSlots(); ++dummySlot) {
      EventContext dummyContext(/*dummyEventNumber*/0, dummySlot);
      std::atomic<bool>* ab = m_forceConfigUpdate.get(dummyContext);
      (*ab) = b;
    }
  }
#else // AnalysisBase
  m_forceConfigUpdate = b;
  ATH_MSG_VERBOSE("The forceForAllSlots flag not used in AnalysisBase, but to stop a compiler warning, this flag is " << forceForAllSlots);
#endif 
}


bool Trig::TrigDecisionTool::getForceConfigUpdate() {
#ifndef XAOD_STANDALONE // AthAnalysis or full Athena
  std::atomic<bool>* ab = m_forceConfigUpdate.get();
  return *ab;
#else // AnalysisBase
  return m_forceConfigUpdate;
#endif 
}



StatusCode Trig::TrigDecisionTool::beginEvent() {

  CacheGlobalMemory* cgmPtr = cgm();
  cgmPtr->setDecisionKeyPtr( &m_decisionKey );
  cgmPtr->setRun2NavigationKeyPtr( &m_navigationKey );
  cgmPtr->setRun3NavigationKeyPtr( &m_HLTSummaryKeyIn );

  size_t slot = 0;
#ifndef XAOD_STANDALONE // AthAnalysis or full Athena
  cgmPtr->setStore(&*evtStore()); // Can change based on event slot
  slot = Gaudi::Hive::currentContext().slot();
#ifndef XAOD_ANALYSIS // Full Athena only
  cgmPtr->setOldDecisionKeyPtr( &m_oldDecisionKey );
  cgmPtr->setOldEventInfoKeyPtr( &m_oldEventInfoKey );
#endif // End Full Athena only
#endif // End AthAnalysis or Full Athena

  // inform the CGM that we are on a new event
  ATH_MSG_VERBOSE("beginEvent: invalidating CacheGlobalMemory");
  cgmPtr->reset_decision();

#ifndef XAOD_STANDALONE // AthAnalysis or full Athena
  if(m_configSvc.empty()) {
#endif
    //for analysis releases we check whether we need to update the config
    //we also do this in full athena, in the case where there was no configSvc provided ...
    ATH_MSG_DEBUG("beginEvent: check if config update is nessecary (via config Tool)");

    bool keysMatch = configKeysMatch(m_configTool->masterKey(),
      m_configTool->lvl1PrescaleKey(),
      m_configTool->hltPrescaleKey());
    
    if(!keysMatch or getForceConfigUpdate()){

      ATH_MSG_INFO("Tool: updating config in slot " << slot
        << " with SMK: " << m_configTool->masterKey() 
        << " and L1PSK: " << m_configTool->lvl1PrescaleKey() 
        << " and HLTPSK: " << m_configTool->hltPrescaleKey()
        << " getForceConfigUpdate()=" << getForceConfigUpdate()
        << " HLT Chains: " << m_configTool->chains().size());

      std::vector<uint32_t>* keys = getKeys();
      keys->resize(3);
      keys->at(0) = m_configTool->masterKey();
      keys->at(1) = m_configTool->lvl1PrescaleKey();
      keys->at(2) = m_configTool->hltPrescaleKey();
      configurationUpdate( &m_configTool->chains(), m_configTool->ctpConfig() );
      setForceConfigUpdate(false);
    } else{
      ATH_MSG_DEBUG("Tool: Cached Trigger configuration keys match for this event in slot " << slot);
    }
#ifndef XAOD_STANDALONE // AthAnalysis or full Athena
  }
#endif

#ifndef XAOD_STANDALONE // AthAnalysis or full Athena
  if(m_configSvc.name() == "xAODConfigSvc" or m_configSvc.name() == "TrigConfigSvc") {
    // ... and where we are using the xAOD service (instead of the TrigConfSvc)
    ATH_MSG_DEBUG("beginEvent: check if config update is nessecary (via " << m_configSvc.name() << ")");

    const uint32_t smk = m_configSvc->masterKey();
    const uint32_t l1psk = m_configSvc->lvl1PrescaleKey();
    const uint32_t hltpsk = m_configSvc->hltPrescaleKey();
    bool keysMatch = configKeysMatch(smk, l1psk, hltpsk);

    if(!keysMatch or getForceConfigUpdate()){

      ATH_MSG_INFO("Svc: updating config in slot " << slot
        << " with SMK: " << smk
        << " and L1PSK: " << l1psk
        << " and HLTPSK: " << hltpsk
        << " getForceConfigUpdate()=" << getForceConfigUpdate()
        << " HLT Chains: " << m_configSvc->chains().size());

      std::vector<uint32_t>* keys = getKeys();
      keys->resize(3);
      keys->at(0) = smk;
      keys->at(1) = l1psk;
      keys->at(2) = hltpsk;
      configurationUpdate( &m_configSvc->chains(), m_configSvc->ctpConfig() );
      setForceConfigUpdate(false);
    }else{
      ATH_MSG_DEBUG("Svc: Cached Trigger configuration keys match for this event in slot " << slot);
    }
  }
#endif

  return StatusCode::SUCCESS;
}

StatusCode Trig::TrigDecisionTool::beginInputFile() {
   // We need to update the cached configuration when switching to a new input
   // file:
   //have to do this at the next beginEvent, because the event info isn't ready at this point (e.g. if the file has no events!)
   ATH_MSG_VERBOSE("Trig::TrigDecisionTool::beginInputFile: setForceConfigUpdate(true, forceForAllSlots=true)");
   setForceConfigUpdate(true, /*forceForAllSlots=*/ true);
   return StatusCode::SUCCESS;
}

bool Trig::TrigDecisionTool::configKeysMatch(uint32_t smk, uint32_t lvl1psk, uint32_t hltpsk){
  std::vector<uint32_t>* keys = getKeys(); // Slot-specific object in full athena.
  if (keys->size() != 3) {
    return false;
  }
  return ( ( smk == keys->at(0) ) &&
           ( lvl1psk == keys->at(1) ) &&
           ( hltpsk == keys->at(2) ) );
}

StatusCode
Trig::TrigDecisionTool::finalize() {
   // release all chaingroups
   m_navigation->reset(true);

   --s_instances;

   ATH_MSG_INFO("Finalized");
   return StatusCode::SUCCESS;
}

#ifndef XAOD_ANALYSIS // Full Athena only
void
Trig::TrigDecisionTool::handle(const Incident& inc) {
   // an update configuration incident triggers the update of the configuration
   ATH_MSG_DEBUG("got  incident type:" << inc.type()  << " source: " << inc.source() );
   
   if ( inc.type()=="TrigConf") {
      if(m_configSvc.isSet()) {
         ATH_MSG_INFO("updating config via config svc");
         configurationUpdate( &m_configSvc->chains(), m_configSvc->ctpConfig());
         setForceConfigUpdate(true, /*forceForAllSlots=*/ true);
      } else {
         ATH_MSG_DEBUG("No TrigConfigSvc, ignoring TrigConf incident.");
      }
   }
   else if (inc.type() == IncidentType::BeginEvent) {
      ATH_MSG_VERBOSE("Obtained IncidentType::BeginEvent via Trig::TrigDecisionTool::handle");
      if (beginEvent().isFailure()) {
         throw std::runtime_error("In Trig::TrigDecisionTool::handle beginEvent() returned StatusCode::FAILURE");
      }
   }
   else if (inc.type() == IncidentType::BeginInputFile) {
      ATH_MSG_VERBOSE("Obtained IncidentType::BeginInputFile via Trig::TrigDecisionTool::handle");
      if (beginInputFile().isFailure()) {
         throw std::runtime_error("In Trig::TrigDecisionTool::handle beginInputFile() returned StatusCode::FAILURE");
      }
   }
   else {
     //call the parent handle
     AsgMetadataTool::handle(inc);
   }
}
#endif

bool
Trig::TrigDecisionTool::isPassed(const std::string& chain, unsigned int condition) const {
   return TrigDecisionToolCore::isPassed(chain, condition);
}


bool
Trig::TrigDecisionTool::isPassed(const std::string& chain) const {
   return TrigDecisionToolCore::isPassed(chain);
}

unsigned int
Trig::TrigDecisionTool::isPassedBits( const std::string& chain ) const {

   return TrigDecisionToolCore::isPassedBits( chain );
}

const std::string&
Trig::TrigDecisionTool::getNavigationFormat() const {
   return m_navigationFormat;
}


