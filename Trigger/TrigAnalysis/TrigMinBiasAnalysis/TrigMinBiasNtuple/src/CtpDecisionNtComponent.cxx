/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMinBiasNtuple/CtpDecisionNtComponent.h"
#include "TrigNtupleBase/NtupleAlgorithm.h"

#include "AnalysisTriggerEvent/CTP_Decision.h"

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/MsgStream.h"

namespace NtComponent {
  
  //---------------------------------------------------------------------

  CtpDecisionNtComponent::CtpDecisionNtComponent(NtupleAlgorithm *mainAlg,
						 NtComponentParameters parameters):
    NtupleComponent(mainAlg, parameters) {
  }
  
  //---------------------------------------------------------------------

  CtpDecisionNtComponent::~CtpDecisionNtComponent() {
  }

  //---------------------------------------------------------------------

  StatusCode CtpDecisionNtComponent::book(void) {

    // Set the key from python or use the default.
    std::map<std::string,std::string>::iterator itr;
    itr = m_parameters.containerKeys.find("CTP_Decision"); // Search for StoreGate class.
    if(itr == m_parameters.containerKeys.end()) {
      m_ctp_DecisionKey = "CTP_Decision"; // The default when no key is found.
    }
    else {
      m_ctp_DecisionKey = (*itr).second; // Use the setting.
    }

    // Create new objects but do not delete them such that they can be
    // stored in the TTree
    m_ctpDecisionItems = new std::vector<std::string>();
    m_ctpDecisionWords = new std::vector<uint32_t>();

    m_tree->Branch("ctpDecisionItems", &m_ctpDecisionItems);
    m_tree->Branch("ctpDecisionWords", &m_ctpDecisionWords);
    m_tree->Branch("ctpDecisionTypeWord", &m_ctpDecisionTypeWord, "ctpDecisionTypeWord/I");

    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------
  
  StatusCode CtpDecisionNtComponent::fill(void) {
    StatusCode sc;
    const CTP_Decision *ctpDecision = 0;

    m_ctpDecisionTypeWord = 999;
    // Clear ntuple vectors
    m_ctpDecisionItems->clear();
    m_ctpDecisionWords->clear();

    sc=(*m_evtStore)->retrieve(ctpDecision, m_ctp_DecisionKey);
    if( sc.isFailure()  || !ctpDecision ) {
      if (!m_warningPrinted) {
	(*m_msg) << MSG::WARNING << m_ctp_DecisionKey << " requested but not found." << endreq;
        m_warningPrinted = true;
      }
	return StatusCode::SUCCESS;
    }

    // Copy item strings
    *m_ctpDecisionItems = ctpDecision->getItems();

    // Copy the trigger result words
    *m_ctpDecisionWords = ctpDecision->getWords();

    // Copy the type word
    m_ctpDecisionTypeWord = ctpDecision->getTriggerTypeWord();

    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------
}
