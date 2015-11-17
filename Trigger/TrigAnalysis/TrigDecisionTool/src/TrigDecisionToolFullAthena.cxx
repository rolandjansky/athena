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
 * @author Lukas Heinrich <lukas.heinrich@cern.ch> - NYU
 *
 ***********************************************************************************/
#include "AsgTools/AsgToolsConf.h"
//only in full Athena
#if defined(ASGTOOL_ATHENA) && !defined(XAOD_ANALYSIS)


#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Incident.h"

#include "TrigConfHLTData/HLTChainList.h"
#include "TrigConfL1Data/CTPConfig.h"
#include "TrigConfL1Data/Menu.h"
#include "GaudiKernel/MsgStream.h"


#include "TrigDecisionTool/DecisionUnpackerStandalone.h"

#include "TrigDecisionTool/DecisionUnpackerAthena.h"
#include "TrigDecisionTool/DecisionUnpackerEventInfo.h"


#include "TrigDecisionTool/TrigDecisionToolFullAthena.h"




static std::vector<std::string> s_instances;


Trig::TrigDecisionTool::TrigDecisionTool(const std::string& name, const std::string& type,
                                         const IInterface* parent) :
   AthAlgTool(name, type, parent),
   m_configSvc("TrigConf::TrigConfigSvc/TrigConfigSvc", name),
   m_navigation("HLT::Navigation/Navigation", this)
{
   declareProperty( "TrigConfigSvc", m_configSvc, "Trigger Config Service");
   declareProperty( "TrigDecisionKey", m_decisionKey="xTrigDecision", "StoreGate key of TrigDecision object. Consult checkSG.py for the actual name.");
   declareProperty( "Navigation", m_navigation, "HLT Navigation tool");
   declareProperty( "PublicChainGroups", m_publicChainGroups, "Pre-created chain groups");
   declareProperty( "UseAODDecision", m_useAODDecision = false );
   declareProperty( "UseEventInfoDecision", m_useEventInfoDecision = false );

   this->outputLevelProperty().declareUpdateHandler(&Trig::TrigDecisionTool::outputlevelupdateHandler, this );
   Logger::setMessaging(this);
   declareInterface<Trig::TrigDecisionTool>(this);
   declareInterface<Trig::ITrigDecisionTool>(this);
}

#ifdef ASGTOOL_ATHENA
void Trig::TrigDecisionTool::outputlevelupdateHandler(Property& p) {
   //call the original update handler
   this->msg_update_handler(p);
   Logger::msg().setLevel(AthMessaging::msg().level());
}
#endif

Trig::TrigDecisionTool::~TrigDecisionTool() {}

StatusCode
Trig::TrigDecisionTool::initialize() {
   TrigDecisionToolCore::initialize().ignore();

   s_instances.push_back(name());
   if ( s_instances.size() > 1 ) {
     ATH_MSG_WARNING("Several TrigDecisionTool instances" );
     ATH_MSG_WARNING("This not to efficent from performance perspective. Access of the same EDM objects will give warnings. Continues anyway ..." );      
   }

   ATH_MSG_INFO("Initializing Trig::TrigDecisionTool");
  
   if (setProperties().isFailure() ) {
     ATH_MSG_ERROR(" error whil setting properties");
     return StatusCode::FAILURE;
   }

   StatusCode sc = m_configSvc.retrieve();
   if ( sc.isFailure() ) {
     ATH_MSG_FATAL("Unable to get pointer to TrigConfigSvc");
     return sc;
   }
   // call update if there is anything in config svc
   if ( m_configSvc->chainList() || m_configSvc->ctpConfig() ) {
      configurationUpdate( m_configSvc->chainList(), m_configSvc->ctpConfig() );
   }

   sc = m_navigation.retrieve();
   if ( sc.isFailure() ) {
     ATH_MSG_FATAL( "Unable to get Navigation tool");
     return sc;
   }
   cgm()->navigation(&*m_navigation);
   cgm()->setStore(&*evtStore());

   ServiceHandle<IIncidentSvc> incSvc("IncidentSvc",name());
   if (incSvc.retrieve().isFailure()) {
     ATH_MSG_ERROR("Cannot retrieve IncidentSvc");
     return StatusCode::FAILURE;
   }

   long int pri=100;
   incSvc->addListener( this, "TrigConf", pri );
   incSvc->addListener( this, "BeginEvent", pri );

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

StatusCode Trig::TrigDecisionTool::beginEvent() {
  ATH_MSG_DEBUG("invalidating object handle");
  if(cgm()->unpacker()){
    cgm()->unpacker()->invalidate_handle();
  }
  return StatusCode::SUCCESS;
}

StatusCode
Trig::TrigDecisionTool::finalize() {
   // release all chaingroups
   m_navigation->reset();
   return StatusCode::SUCCESS;
}


void
Trig::TrigDecisionTool::handle(const Incident& inc) {
   // an update configuration incident triggers the update of the configuration
  ATH_MSG_VERBOSE("got  incident type:" << inc.type()  << " source: " << inc.source() );

   if ( inc.type()=="TrigConf" ) {
      configurationUpdate( m_configSvc->chainList(), 
                           m_configSvc->ctpConfig() );
   }

   if ( inc.type()=="BeginEvent"){
     StatusCode sc = beginEvent();
     if(sc.isFailure()){
       ATH_MSG_WARNING("handling of BeginEvent failed");
     }
   }
}


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

#endif // full AthenaEnv
