/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMinBiasNtuple/ZdcLvl1NtComponent.h"
#include "TrigNtupleBase/NtupleAlgorithm.h"

#include "TrigT1Interfaces/ZdcCTP.h"
#include "TrigT1Interfaces/TrigT1StoreGateKeys.h"

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/MsgStream.h"

namespace NtComponent {
  
  //---------------------------------------------------------------------

  ZdcLvl1NtComponent::ZdcLvl1NtComponent(NtupleAlgorithm *mainAlg,
					 NtComponentParameters parameters):
    NtupleComponent(mainAlg, parameters) {
  }
  
  //---------------------------------------------------------------------

  ZdcLvl1NtComponent::~ZdcLvl1NtComponent() {
  }

  //---------------------------------------------------------------------

  StatusCode ZdcLvl1NtComponent::book(void) {

    // Set the key from python or use the default.
    std::map<std::string,std::string>::iterator itr;
    itr = m_parameters.containerKeys.find("ZdcCTP"); // Search for StoreGate class.
    if(itr == m_parameters.containerKeys.end()) {
      m_zdcCtpKey = LVL1::DEFAULT_ZdcCTPLocation; // The default when no key is found.
    }
    else {
      m_zdcCtpKey = (*itr).second; // Use the setting.
    }

    m_tree->Branch("lvl1_zdc_cableWord", &m_zdcCableWord, "lvl1_zdc_cableWord/i");

    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------
  
  StatusCode ZdcLvl1NtComponent::fill(void) {
    StatusCode sc;
    m_zdcCableWord = 0;

    const LVL1::ZdcCTP *zdcCTP = 0;
    sc = (*m_evtStore)->retrieve(zdcCTP, m_zdcCtpKey);
    if (sc.isFailure() || !zdcCTP) {
      if (!m_warningPrinted) {
	(*m_msg) << MSG::WARNING <<  m_zdcCtpKey <<  " requested but not found." << endreq;
	m_warningPrinted = true;
      }
      return StatusCode::SUCCESS;
    }
    m_zdcCableWord = zdcCTP->cableWord0();
    
    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------
}
