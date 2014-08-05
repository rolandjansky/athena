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
 *
 ***********************************************************************************/

#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Incident.h"

#include "TrigConfHLTData/HLTChainList.h"
#include "TrigConfL1Data/CTPConfig.h"
#include "TrigConfL1Data/Menu.h"
#include "GaudiKernel/MsgStream.h"


#include "TrigDecisionTool/DecisionObjectHandle.h"
#include "TrigDecisionTool/TrigDecisionTool.h"

static std::vector<std::string> s_instances;


Trig::TrigDecisionTool::TrigDecisionTool(const std::string& name, const std::string& type,
                                         const IInterface* parent) :
   AlgTool(name, type, parent),
   TrigDecisionToolCore(name),
   m_configSvc("TrigConf::TrigConfigSvc/TrigConfigSvc", name),
   m_store("StoreGateSvc", name),
   m_navigation("HLT::Navigation/Navigation", this)
{
   declareProperty( "TrigConfigSvc", m_configSvc, "Trigger Config Service");
   declareProperty( "TrigDecisionKey", m_decisionKey="TrigDecision", "StoreGate key of TrigDecision object. Consult checkSG.py for the actual name.");
   declareProperty( "Navigation", m_navigation, "HLT Navigation tool");
   declareProperty( "PublicChainGroups", m_publicChainGroups, "Pre-created chain groups");
   declareProperty( "EvtStore", m_store );

   declareInterface<Trig::TrigDecisionTool>(this);
   declareInterface<Trig::ITrigDecisionTool>(this);
}

Trig::TrigDecisionTool::~TrigDecisionTool() {}

StatusCode
Trig::TrigDecisionTool::initialize() {
   TrigDecisionToolCore::initialize().ignore();

    
   Trig::Logger::updateMsgStream(new MsgStream( msgSvc(), name()));   

   s_instances.push_back(name());
   if ( s_instances.size() > 1 ) {
      log() << MSG::WARNING << "Several TrigDecisionTool instances: " << s_instances << endreq;
      log() << MSG::WARNING << "This not to efficent from performance perspective. Access of the same EDM objects will give warnings. Continues anyway ..." << endreq;      
   }

   if (msgLvl(MSG::INFO))
      log() << MSG::INFO << "Initializing Trig::TrigDecisionTool" << endreq;
  
   if (setProperties().isFailure() ) {
      log() << MSG::ERROR << "setting properties" << endreq;
      return StatusCode::FAILURE;
   }

   if ( m_store.retrieve().isFailure() ) {
      log() << MSG::ERROR << "retrieving StoreGateSvc" << endreq;
      return StatusCode::FAILURE;
   }

  

   StatusCode sc = m_configSvc.retrieve();
   if ( sc.isFailure() ) {
      log() << MSG::FATAL << "Unable to get pointer to TrigConfigSvc" << endreq;
      return sc;
   }
   // call update if there is anything in config svc
   if ( m_configSvc->chainList() || m_configSvc->ctpConfig() ) {
      configurationUpdate( m_configSvc->chainList(), m_configSvc->ctpConfig() );
   }

   sc = m_navigation.retrieve();
   if ( sc.isFailure() ) {
      log() << MSG::FATAL << "Unable to get Navigation tool" << endreq;
      return sc;
   }
   cgm()->navigation(&*m_navigation);
   cgm()->decisionHandle(new DecisionObjectHandleAthena(&*m_store, m_decisionKey));
 
   ServiceHandle<IIncidentSvc> incSvc("IncidentSvc",name());
   if (incSvc.retrieve().isFailure()) {
      log() << MSG::WARNING << "Cannot retrieve IncidentSvc" << endreq;
      return StatusCode::FAILURE;
   }

   long int pri=100;
   incSvc->addListener( this, "TrigConf", pri );

   if (msgLvl(MSG::INFO))
      log() << MSG::INFO << "Initialized TDT" << endreq;

   std::map<std::string, std::string>::const_iterator pIt;
   for ( pIt = m_publicChainGroups.begin(); pIt != m_publicChainGroups.end(); ++pIt ) {
      std::vector<std::string> patterns;
      patterns.push_back(pIt->second);
      cgm()->createChainGroup(patterns, pIt->first);
      if (msgLvl(MSG::INFO))
         log() << MSG::INFO << "created Public Chain Group " << pIt->first << " with pattern: " << pIt->second << endreq;
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
   if (msgLvl(MSG::INFO))
      log() << MSG::INFO << "got  incident type:" << inc.type()  << " source: " << inc.source() << endreq;

   if (msgLvl(MSG::DEBUG))
      log() << MSG::DEBUG << "got  incident " << endreq;

   if ( inc.type()=="TrigConf" ) {
      configurationUpdate( m_configSvc->chainList(), 
                           m_configSvc->ctpConfig() );
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
