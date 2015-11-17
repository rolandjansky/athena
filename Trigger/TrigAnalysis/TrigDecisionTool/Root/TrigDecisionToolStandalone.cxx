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
//never in full Athena
#if defined(ASGTOOL_ATHENA) && !defined(XAOD_ANALYSIS)
#else


#if defined(ASGTOOL_ATHENA) && !defined(XAOD_ANALYSIS)
#include "TrigDecisionTool/DecisionUnpackerAthena.h"
#endif


#include "TrigDecisionTool/DecisionUnpackerStandalone.h"
#include "TrigNavStructure/StandaloneNavigation.h"
#include "TrigDecisionTool/TrigDecisionToolStandalone.h"
#include "TrigConfHLTData/HLTChainList.h"
#include "TrigConfL1Data/CTPConfig.h"
#include "TrigConfL1Data/Menu.h"




static std::vector<std::string> s_instances;


Trig::TrigDecisionTool::TrigDecisionTool(const std::string& name) :
  asg::AsgMetadataTool(name),
  m_configKeysCache(),
  m_configTool("TrigConf::xAODConfigTool"),
  m_configKeysCached( false )
#ifndef XAOD_ANALYSIS
  ,m_configSvc("TrigConf::xAODConfigSvc", name)
  ,m_fullNavigation("HLT::Navigation/Navigation", this)
  ,m_navigation(0) //should initialize it... it's dangerous not to
#else
  ,m_navigation(new HLT::StandaloneNavigation())
#endif
{
   declareProperty( "TrigDecisionKey", m_decisionKey="xTrigDecision", "StoreGate key of TrigDecision object. Consult checkSG.py for the actual name.");
   declareProperty( "PublicChainGroups", m_publicChainGroups, "Pre-created chain groups");
   declareProperty( "UseAODDecision", m_useAODDecision = false );
   declareProperty( "ConfigTool", m_configTool);


   //full Athena env
#ifndef XAOD_ANALYSIS
   declareProperty( "TrigConfigSvc", m_configSvc, "Trigger Config Service");
   declareProperty( "Navigation", m_fullNavigation); 
   m_navigation = &*m_fullNavigation; 
#endif

   //just for Athena/AthAnalysisBase
#ifndef XAOD_STANDALONE
   this->outputLevelProperty().declareUpdateHandler(&Trig::TrigDecisionTool::outputlevelupdateHandler, this );
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
   if ( s_instances.size() > 1 ) {
     ATH_MSG_WARNING("Several TrigDecisionTool instances" );
     ATH_MSG_WARNING("This not to efficent from performance perspective. Access of the same EDM objects will give warnings. Continues anyway ..." );      
   }

   ATH_MSG_INFO("Initializing Trig::TrigDecisionTool");
  
#if defined(ASGTOOL_ATHENA) && !defined(XAOD_ANALYSIS)
   StatusCode sc = m_configSvc.retrieve();
   if ( sc.isFailure() ) {
     ATH_MSG_FATAL("Unable to get pointer to TrigConfigSvc");
     return sc;
   }
   // call update if there is anything in config svc
   if ( m_configSvc->chainList() || m_configSvc->ctpConfig() ) {
      configurationUpdate( m_configSvc->chainList(), m_configSvc->ctpConfig() );
   }


   sc = m_fullNavigation.retrieve();
   if ( sc.isFailure() ) {
     ATH_MSG_FATAL( "Unable to get Navigation tool");
     return sc;
   }
#endif

   ATH_CHECK(m_configTool.retrieve());

   cgm()->navigation(&*m_navigation);
   cgm()->setStore(&*evtStore());

   if(m_useAODDecision){
     ATH_MSG_INFO("using old decision");
#if defined(ASGTOOL_ATHENA) && !defined(XAOD_ANALYSIS)
     DecisionUnpackerAthena* unpacker = new DecisionUnpackerAthena(&*evtStore(), m_decisionKey);
     cgm()->setUnpacker(unpacker);
#else
     ATH_MSG_WARNING("old decision only available in Athena proper");
#endif
   }
   else{
     ATH_MSG_INFO("using new decision");
     DecisionUnpackerStandalone* unpacker = new DecisionUnpackerStandalone(&*evtStore(),m_decisionKey,"TrigNavigation");
     cgm()->setUnpacker(unpacker);
   }

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
  //#if defined(ASGTOOL_STANDALONE) || defined(XAOD_ANALYSIS)
  ATH_MSG_DEBUG("beginEvent: updating config ");
  //invalidate handle so that we read a new decision object
  cgm()->unpacker()->invalidate_handle();


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
    ATH_MSG_DEBUG("keysmatch: " << keysMatch << " cached: " << m_configKeysCached);
  }
  //#endif
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
   return StatusCode::SUCCESS;
}

#ifndef XAOD_ANALYSIS
void
Trig::TrigDecisionTool::handle(const Incident& inc) {
   // an update configuration incident triggers the update of the configuration
  ATH_MSG_VERBOSE("got  incident type:" << inc.type()  << " source: " << inc.source() );

   if ( inc.type()=="TrigConf" ) {
      configurationUpdate( m_configSvc->chainList(), 
                           m_configSvc->ctpConfig() );
   }
   /*
   if ( inc.type()=="BeginEvent"){
     StatusCode sc = beginEvent();
     if(sc.isFailure()){
       ATH_MSG_WARNING("handling of BeginEvent failed");
     }
     }*/
   //call the parent handle 
   AsgMetadataTool::handle(inc);
   
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


#endif // never in full Athena
