/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

#include "AsgTools/AsgToolsConf.h"
#include "TrigDecisionTool/DecisionUnpackerAthena.h"



#include "TrigDecisionTool/DecisionUnpackerStandalone.h"
#include "TrigNavStructure/StandaloneNavigation.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TrigConfHLTData/HLTChainList.h"
#include "TrigConfL1Data/CTPConfig.h"
#include "TrigConfL1Data/Menu.h"




static std::vector<std::string> s_instances;


Trig::TrigDecisionTool::TrigDecisionTool(const std::string& name) :
  asg::AsgMetadataTool(name),
  m_configKeysCache(),
  m_configKeysCached( false )
  ,m_configTool("TrigConf::xAODConfigTool")
#ifndef XAOD_ANALYSIS
  ,m_configSvc("", name) //defaults to empty, which will make full athena use configTool ... only works with xAOD
  ,m_fullNavigation("HLT::Navigation/Navigation", this)
  ,m_navigation(0) //should initialize it... it's dangerous not to
#else
  ,m_navigation(new HLT::StandaloneNavigation())
#endif
{
   declareProperty( "TrigDecisionKey", m_decisionKey="xTrigDecision", "StoreGate key of TrigDecision object. Consult checkSG.py for the actual name.");
   declareProperty( "PublicChainGroups", m_publicChainGroups, "Pre-created chain groups");
   declareProperty( "UseAODDecision", m_useAODDecision = false );
   declareProperty( "AcceptMultipleInstance", m_acceptMultipleInstance = false );

   //full Athena env
#ifndef XAOD_ANALYSIS
   declareProperty( "TrigConfigSvc", m_configSvc, "Trigger Config Service");
   declareProperty( "Navigation", m_fullNavigation); 
   m_navigation = &*m_fullNavigation; 
#endif
   declareProperty( "ConfigTool", m_configTool);


#ifndef XAOD_STANDALONE
   //just for Athena/AthAnalysisBase
   auto props = getProperties();
   for( Property* prop : props ) {
     if( prop->name() != "OutputLevel" ) {
       continue;
     }
     prop->declareUpdateHandler( &Trig::TrigDecisionTool::outputlevelupdateHandler, this );
     break;
   }
#endif
    Logger::setMessaging(this);

   m_configKeysCache.resize(3,0);
}

#ifdef ASGTOOL_ATHENA
void Trig::TrigDecisionTool::outputlevelupdateHandler(Property& p) {
   //call the original update handler
   this->msg_update_handler(p);
   Logger::msg().setLevel(AthMessaging::msg().level());
}
#endif

Trig::TrigDecisionTool::~TrigDecisionTool() {
#ifdef XAOD_ANALYSIS
  if(m_navigation){
    delete m_navigation;
  }
#endif
}

StatusCode
Trig::TrigDecisionTool::initialize() {
   TrigDecisionToolCore::initialize().ignore();

   s_instances.push_back(name());
   if ( s_instances.size() > 1) {
       ATH_MSG_WARNING("Several TrigDecisionTool instances" );
       ATH_MSG_WARNING("This not to efficent from performance perspective. Access of the same EDM objects will give warnings. Continues anyway ..." );      
       if (!m_acceptMultipleInstance){
	 ATH_MSG_ERROR("Will not accept multiple instances. If you really want to have some, use 'AcceptMultipleInstance' property" );
	 return StatusCode::FAILURE;
       }
   }

   ATH_MSG_INFO("Initializing Trig::TrigDecisionTool (standalone version even for athena)");
  
#if defined(ASGTOOL_ATHENA) && !defined(XAOD_ANALYSIS)
   //This is the full Athena Environment
   //we setup the full TrigConfigSvc
   
   if(m_configSvc.empty()) {
     ATH_MSG_DEBUG("No TrigConfigSvc provided. Using ConfigTool instead...");
     CHECK( m_configTool.retrieve()); //use configTool if no configSvc available
   } else {

   StatusCode sc = m_configSvc.retrieve();
   if ( sc.isFailure() ) {
     ATH_MSG_FATAL("Unable to get pointer to TrigConfigSvc");
     return sc;
   }
   // call update if there is anything in config svc
   if ( m_configSvc->chainList() || m_configSvc->ctpConfig() ) {
      configurationUpdate( m_configSvc->chainList(), m_configSvc->ctpConfig() );
   }
   }


   StatusCode sc = m_fullNavigation.retrieve();
   if ( sc.isFailure() ) {
     ATH_MSG_FATAL( "Unable to get Navigation tool");
     return sc;
   }
#else
   ATH_CHECK(m_configTool.retrieve());
#endif



   cgm()->navigation(&*m_navigation);
   cgm()->setStore(&*evtStore());
   
#ifdef ASGTOOL_ATHENA
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

   m_configKeysCached = false;
  
   return StatusCode::SUCCESS;
}

StatusCode Trig::TrigDecisionTool::beginEvent() {
  //invalidate handle so that we read a new decision object
  if(cgm()->unpacker()){
    ATH_MSG_VERBOSE("beginEvent: invalidating handle");
    cgm()->unpacker()->invalidate_handle();
  }
 

#ifndef XAOD_ANALYSIS
  if(m_configSvc.empty()) {
#endif
  //for analysis releases we check whether we need to update the config
    //we also do this in full athena, in the case where there was no configSvc provided
  ATH_MSG_VERBOSE("beginEvent: check if config update is nessecary (via config Tool)");
  
  bool keysMatch = configKeysMatch(m_configTool->masterKey(),m_configTool->lvl1PrescaleKey(),m_configTool->hltPrescaleKey());
  if(!m_configKeysCached || !keysMatch){
    ATH_MSG_INFO("updating config with SMK: " << m_configTool->masterKey() << " and L1PSK: " << m_configTool->lvl1PrescaleKey() << " and HLTPSK: " << m_configTool->hltPrescaleKey());
    
    
    m_configKeysCache[0]    = m_configTool->masterKey();
    m_configKeysCache[1]    = m_configTool->lvl1PrescaleKey();     
    m_configKeysCache[2]    = m_configTool->hltPrescaleKey();
    m_configKeysCached = true;
    configurationUpdate( m_configTool->chainList(), m_configTool->ctpConfig() );
  }
  else{
    ATH_MSG_VERBOSE("keysmatch: " << keysMatch << " cached: " << m_configKeysCached);
  }
#ifndef XAOD_ANALYSIS
  }
#endif
  return StatusCode::SUCCESS;
}

StatusCode Trig::TrigDecisionTool::beginInputFile() {
  
   // We need to update the cached configuration when switching to a new input
   // file:
   //have to do this at the next beginEvent, because the event info isn't ready at this point (e.g. if the file has no events!)
   m_configKeysCached=false;

   // Return gracefully:
   return StatusCode::SUCCESS;
}

bool Trig::TrigDecisionTool::configKeysMatch(uint32_t smk, uint32_t lvl1psk, uint32_t hltpsk){
  return ( ( smk == m_configKeysCache[ 0 ] ) &&
           ( lvl1psk == m_configKeysCache[ 1 ] ) &&
           ( hltpsk == m_configKeysCache[ 2 ] ) );
}

StatusCode
Trig::TrigDecisionTool::finalize() {
   // release all chaingroups
   m_navigation->reset();

   auto it = std::find(s_instances.begin(), s_instances.end(), name());
   if(it != s_instances.end()){
     s_instances.erase(it);
   }
   else{
      ATH_MSG_ERROR("could not find instance name in instance list, but must have been added in ::initialize(). Name: " << name());
      return StatusCode::FAILURE;
   }
   ATH_MSG_INFO("TDT finalized and removed from instance list");
   return StatusCode::SUCCESS;
}

#ifndef XAOD_ANALYSIS
void
Trig::TrigDecisionTool::handle(const Incident& inc) {
   // an update configuration incident triggers the update of the configuration
   ATH_MSG_DEBUG("got  incident type:" << inc.type()  << " source: " << inc.source() );
   
   if ( inc.type()=="TrigConf" ) {
     ATH_MSG_INFO("updating config via config svc");
     
     
     configurationUpdate( m_configSvc->chainList(), 
     			  m_configSvc->ctpConfig() 
     			  );
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


