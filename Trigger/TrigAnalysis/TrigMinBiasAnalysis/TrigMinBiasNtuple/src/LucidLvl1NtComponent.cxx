/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMinBiasNtuple/LucidLvl1NtComponent.h"
#include "TrigNtupleBase/NtupleAlgorithm.h"

#include "TrigT1Interfaces/LucidCTP.h"
#include "TrigT1Interfaces/TrigT1StoreGateKeys.h"

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/MsgStream.h"

namespace NtComponent {
  
  //---------------------------------------------------------------------

  LucidLvl1NtComponent::LucidLvl1NtComponent(NtupleAlgorithm *mainAlg,
					     NtComponentParameters parameters):
    NtupleComponent(mainAlg, parameters) {
  }
  
  //---------------------------------------------------------------------

  LucidLvl1NtComponent::~LucidLvl1NtComponent() {
  }

  //---------------------------------------------------------------------

  StatusCode LucidLvl1NtComponent::book(void) {
    // Set the key from python or use the default.
    std::map<std::string,std::string>::iterator itr;
    itr = m_parameters.containerKeys.find("LucidCTP"); // Search for StoreGate class.
    if(itr == m_parameters.containerKeys.end()) {
      m_lucidCtpKey = LVL1::DEFAULT_LucidCTPLocation; // The default when no key is found.
    }
    else {
      m_lucidCtpKey = (*itr).second; // Use the setting.
    }

    m_tree->Branch("lvl1_lucid_cableWord", &m_lucidCableWord, "lvl1_lucid_cableWord/i");

    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------
  
  StatusCode LucidLvl1NtComponent::fill(void) {
    StatusCode sc;

    const LVL1::LucidCTP *lucidCTP = 0;
    m_lucidCableWord = 999;

    sc = (*m_evtStore)->retrieve(lucidCTP, m_lucidCtpKey);
    if (sc.isFailure() || !lucidCTP) {
      if (!m_warningPrinted) {
	(*m_msg) << MSG::WARNING
		 << m_lucidCtpKey << " requested but not found."
		 << endreq; 
      }
      return StatusCode::SUCCESS;
    }
    m_lucidCableWord = lucidCTP->cableWord0();
    
    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------
}
