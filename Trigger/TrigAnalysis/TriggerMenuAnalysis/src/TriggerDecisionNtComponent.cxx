/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TriggerMenuAnalysis/TriggerDecisionNtComponent.h"
#include "TrigNtupleBase/NtupleAlgorithm.h"


namespace NtComponent {
  
  //---------------------------------------------------------------------

  TriggerDecisionNtComponent::TriggerDecisionNtComponent(NtupleAlgorithm *mainAlg,
					     NtComponentParameters parameters):
    NtupleComponent(mainAlg, parameters), m_useDecision(true) {
    m_allEF = 0;
    m_allL2 = 0;
    m_allL1 = 0;
    m_level1TriggerPasses = 0;
    m_level2TriggerPasses = 0;
    m_eventFilterTriggerPasses = 0;
  }
  
  //---------------------------------------------------------------------

  TriggerDecisionNtComponent::~TriggerDecisionNtComponent() {
  }

  //---------------------------------------------------------------------

  StatusCode TriggerDecisionNtComponent::book(void) {
    m_level1TriggerPasses = new std::vector<std::string>();
    m_level2TriggerPasses = new std::vector<std::string>();
    m_eventFilterTriggerPasses = new std::vector<std::string>();
    m_tree->Branch("trigger_dec_lvl1_trig_names", &m_level1TriggerPasses);
    m_tree->Branch("trigger_dec_lvl2_trig_names", &m_level2TriggerPasses);
    m_tree->Branch("trigger_dec_evt_filter_trig_names", &m_eventFilterTriggerPasses);

    StatusCode sc = m_mainAlg->trigDecisionTool().retrieve();
    if ( sc.isFailure()   ) {  
     	(*m_msg) << MSG::WARNING  << "TriggerDecisionNtComponent FATAL: Could not retrieve TrigDecisionTool!" << endreq;
      m_useDecision = false;
      m_allL1 = 0; 
      m_allL2 = 0; 
      m_allEF = 0; 
    } else {
      // We could also select only the minimum bias triggers with e.g.
      // const Trig::ChainGroup* allL2 = m_mainAlg->trigDecisionTool()->getChainGroup("L2_mb.*");
      m_allL1 = m_mainAlg->trigDecisionTool()->getChainGroup("L1.*");
      m_allL2 = m_mainAlg->trigDecisionTool()->getChainGroup("L2.*");
      m_allEF = m_mainAlg->trigDecisionTool()->getChainGroup("EF.*");

    }
    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------
  
  StatusCode TriggerDecisionNtComponent::fill(void) {
   
    // clear variables
    m_level1TriggerPasses->clear();
    m_level2TriggerPasses->clear();
    m_eventFilterTriggerPasses->clear();

    // if the TrigDecisionTool is not there, don't try to use it!
    if (!m_useDecision) return StatusCode::SUCCESS;

    std::vector<std::string> names;
    if (m_allEF) names = m_allEF->getListOfTriggers();
    
    (*m_msg) << MSG::DEBUG << "TriggerDecisionNtComponent: Number of EF triggers retrieved " << names.size() <<endreq;
    for (unsigned int loop = 0 ; loop < names.size() ; loop++) {
      if ( m_mainAlg->trigDecisionTool()->isPassed(names[loop])){
	(*m_msg) << MSG::VERBOSE << "TriggerDecisionNtComponent: Trigger '" << names[loop]<<"' passed and is being added to ntuple" <<endreq;
	m_eventFilterTriggerPasses->push_back(names[loop]);
	 }
    }

    names.clear();
    if (m_allL2) names = m_allL2->getListOfTriggers();
    (*m_msg) << MSG::DEBUG << "TriggerDecisionNtComponent: Number of L2 triggers retrieved " << names.size() <<endreq;
    for (unsigned int loop = 0 ; loop < names.size() ; loop++) {
      if ( m_mainAlg->trigDecisionTool()->isPassed(names[loop])){
	(*m_msg) << MSG::VERBOSE << "TriggerDecisionNtComponent: Trigger '" << names[loop]<<"' passed and is being added to ntuple" <<endreq;
	m_level2TriggerPasses->push_back(names[loop]); 
	 }
    }

    names.clear();
    if (m_allL1) names = m_allL1->getListOfTriggers();
    (*m_msg) << MSG::DEBUG << "TriggerDecisionNtComponent: Number of L1 triggers retrieved " << names.size() <<endreq;
    for (unsigned int loop = 0 ; loop < names.size() ; loop++) {
      if ( m_mainAlg->trigDecisionTool()->isPassed(names[loop])){
	(*m_msg) << MSG::VERBOSE << "TriggerDecisionNtComponent: Trigger '" << names[loop]<<"' passed and is being added to ntuple" <<endreq;
	m_level1TriggerPasses->push_back(names[loop]); 
	 }
    }


    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------
}
