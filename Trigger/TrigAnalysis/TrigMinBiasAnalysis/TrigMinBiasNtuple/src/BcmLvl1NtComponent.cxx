/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMinBiasNtuple/BcmLvl1NtComponent.h"
#include "TrigNtupleBase/NtupleAlgorithm.h"

#include "TrigT1Interfaces/BcmCTP.h"
#include "TrigT1Interfaces/TrigT1StoreGateKeys.h"

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/MsgStream.h"

namespace NtComponent {
  
  //---------------------------------------------------------------------

  BcmLvl1NtComponent::BcmLvl1NtComponent(NtupleAlgorithm *mainAlg,
					 NtComponentParameters parameters):
    NtupleComponent(mainAlg, parameters) {
  }
  
  //---------------------------------------------------------------------

  BcmLvl1NtComponent::~BcmLvl1NtComponent() {
  }

  //---------------------------------------------------------------------

  StatusCode BcmLvl1NtComponent::book(void) {

    // Set the key from python or use the default.
    std::map<std::string,std::string>::iterator itr;
    itr = m_parameters.containerKeys.find("BcmCTP"); // Search for StoreGate class.
    if(itr == m_parameters.containerKeys.end()) {
      m_bcmCtpKey = LVL1::DEFAULT_BcmCTPLocation; // The default when no key is found.
    }
    else {
      m_bcmCtpKey = (*itr).second; // Use the setting. 
    }

    m_tree->Branch("lvl1_bcm_cableWord", &m_bcmCableWord, "lvl1_bcm_cableWord/i");

    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------
  
  StatusCode BcmLvl1NtComponent::fill(void) {
    StatusCode sc;

    const LVL1::BcmCTP *bcmCTP = 0;

    m_bcmCableWord = 999;

    sc = (*m_evtStore)->retrieve(bcmCTP, m_bcmCtpKey);
    if (sc.isFailure() || !bcmCTP) {
      if (!m_warningPrinted) {
	(*m_msg) << MSG::WARNING
		 << m_bcmCtpKey << " requested but not found."
		 << endreq; 
	m_warningPrinted = true;
      }
      return StatusCode::SUCCESS;
    }
    m_bcmCableWord = bcmCTP->cableWord0();
    
    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------
}
