/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$

// Gaudi/Athena include(s):
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IIncidentListener.h"
#include "AthenaKernel/errorcheck.h"

// Local include(s):
#include "TrigObjectMatching/TrigMatchTool.h"

/**********************************************************************
 *
 * Constructors and destructors
 *
 **********************************************************************/

TrigMatchTool::TrigMatchTool( const std::string& type,
                              const std::string& name,
                              const IInterface* parent )
   : AthAlgTool(type, name, parent),
     m_trigDecisionTool( "Trig::TrigDecisionTool/TrigDecisionTool" ) {

   declareInterface<TrigMatchTool>(this);

   declareProperty( "TrigDecisionTool", m_trigDecisionTool );
}

/**********************************************************************
 *
 * Initialization and Finalization 
 *
 **********************************************************************/

StatusCode TrigMatchTool::initialize() {

   ATH_MSG_DEBUG( "TrigMatchTool::Intialize()" );

   // load the required tool(s):
   CHECK( m_trigDecisionTool.retrieve() );
   ATH_MSG_INFO( "Successfully retrieved the TrigDecisionTool!" );
   this->setTDT( &( *m_trigDecisionTool ) );

   // register to hear end event incidents so we can clear
   // the cache
   ServiceHandle<IIncidentSvc> p_incSvc("IncidentSvc", this->name());
   p_incSvc->addListener( this, "EndEvent" );
   p_incSvc->addListener( this, "TrigConf" );

   return StatusCode::SUCCESS;
}

/**********************************************************************
 *
 * Private functions
 *
 **********************************************************************/

void TrigMatchTool::assertConfiguredChainNames() {

   if( m_chainNames.size() || ! m_trigDecisionTool ) {
      return;
   }
   
   // grab the chain names from the tdt - note that this
   // includes the L1 items
   m_chainNames = m_trigDecisionTool->getListOfTriggers();
   m_nConfiguredChainNames = m_chainNames.size();

   buildChainIndexMap();

   return;
}

std::string TrigMatchTool::lowerChainName( const std::string& chainName ) {

   // enable expert methods in tdt
   Trig::ExpertMethods *em = m_trigDecisionTool->ExperimentalAndExpertMethods();
   em->enable();

   const TrigConf::HLTChain *hltChain = em->getChainConfigurationDetails(chainName);
   if( hltChain ) {
      return hltChain->lower_chain_name();
   }

   return "";
}

// issue a clear at the beginning of each event
void TrigMatchTool::handle( const Incident &inc ) {

   if (inc.type() == "EndEvent") {
      this->endEvent();
   }
   if( inc.type() == "TrigConf" ) {
      ATH_MSG_INFO( "Switching to a new trigger configuration" );
      // Clear and re-build all cached information. (The LVL1->LVL2 mapping
      // is re-built the next time it is needed.)
      m_chainNames.clear();
      assertConfiguredChainNames();
      m_l1l2Map.clear();
      buildL1L2Map();
   }

   return;
}
