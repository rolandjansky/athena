/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMinBiasNtuple/MbtsLvl1NtComponent.h"
#include "TrigNtupleBase/NtupleAlgorithm.h"

#include "TrigT1Interfaces/MbtsCTP.h"
#include "TrigT1Interfaces/TrigT1StoreGateKeys.h"

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/MsgStream.h"

namespace NtComponent {
  
  //---------------------------------------------------------------------

  MbtsLvl1NtComponent::MbtsLvl1NtComponent(NtupleAlgorithm *mainAlg,
					   NtComponentParameters parameters):
    NtupleComponent(mainAlg, parameters) {
  }
  
  //---------------------------------------------------------------------

  MbtsLvl1NtComponent::~MbtsLvl1NtComponent() {
  }

  //---------------------------------------------------------------------

  StatusCode MbtsLvl1NtComponent::book(void) {

    m_mbtsCtpAinputContainerName = LVL1::DEFAULT_MbtsACTPLocation;
    m_mbtsCtpCinputContainerName = LVL1::DEFAULT_MbtsCCTPLocation;

    m_tree->Branch("lvl1_mbts_cableWordA", &m_mbtsCableWordA, "lvl1_mbts_cableWordA/i");
    m_tree->Branch("lvl1_mbts_cableWordC", &m_mbtsCableWordC, "lvl1_mbts_cableWordC/i");

    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------
  
  StatusCode MbtsLvl1NtComponent::fill(void) {
    StatusCode sc;

    m_mbtsCableWordA = 0;
    m_mbtsCableWordC = 0;

    const LVL1::MbtsCTP *mbtsCTP_A = 0;
    sc = (*m_evtStore)->retrieve(mbtsCTP_A, m_mbtsCtpAinputContainerName);
    if (sc.isFailure() || !mbtsCTP_A) {
      if (!m_warningPrinted) {
	(*m_msg) << MSG::WARNING
		 << m_mbtsCtpAinputContainerName << " requested but not found."
		 << endreq; 
	m_warningPrinted = true;
      }
      return StatusCode::SUCCESS;
    }
    m_mbtsCableWordA = mbtsCTP_A->cableWord0();
    
    const LVL1::MbtsCTP *mbtsCTP_C = 0;
    sc = (*m_evtStore)->retrieve(mbtsCTP_C, m_mbtsCtpCinputContainerName);
    if (sc.isFailure() || !mbtsCTP_C) {
      if (!m_warningPrinted) {
	(*m_msg) << MSG::WARNING
		 << m_mbtsCtpCinputContainerName << " requested but not found."
		 << endreq; 
	m_warningPrinted = true;
      }
      return StatusCode::SUCCESS;
    }
    m_mbtsCableWordC = mbtsCTP_C->cableWord0();
    
    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------
}
