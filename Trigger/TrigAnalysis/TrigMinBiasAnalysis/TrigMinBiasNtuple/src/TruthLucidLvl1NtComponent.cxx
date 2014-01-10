/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMinBiasNtuple/TruthLucidLvl1NtComponent.h"
#include "TrigNtupleBase/NtupleAlgorithm.h"

#include "LUCID_RawEvent/LUCID_DigitContainer.h"

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/MsgStream.h"

#include <vector>

namespace NtComponent {
  
  //---------------------------------------------------------------------

  TruthLucidLvl1NtComponent::TruthLucidLvl1NtComponent(NtupleAlgorithm *mainAlg,
						       NtComponentParameters parameters):
    NtupleComponent(mainAlg, parameters) {
  }
  
  //---------------------------------------------------------------------

  TruthLucidLvl1NtComponent::~TruthLucidLvl1NtComponent() {
  }

  //---------------------------------------------------------------------

  StatusCode TruthLucidLvl1NtComponent::book(void) {

    // Set the key from python or use the default.
    std::map<std::string,std::string>::iterator itr;
    itr = m_parameters.containerKeys.find("LUCID_DigitContainer"); // Search for StoreGate class.
    if(itr == m_parameters.containerKeys.end()) {
      m_lucid_DigitContainer_key = "Lucid_Digits"; // The default when no key is found.
    }
    else {
      m_lucid_DigitContainer_key = (*itr).second; // Use the setting.
    }

    // Create new objects but do not delete them such that they can be
    // stored in the TTree

    m_side   = new std::vector<short>();
    m_tubeId = new std::vector<short>();
    m_npe    = new std::vector<double>();
    m_npeGas = new std::vector<int>();
    m_npePmt = new std::vector<int>();
    m_qdc    = new std::vector<int>();
    m_tdc    = new std::vector<int>();
    m_isHit  = new std::vector<bool>();

    m_tree->Branch("lucidDigit_ntube" , &m_ntube, "lucidDigit_ntube/I");
    m_tree->Branch("lucidDigit_side"  , &m_side);
    m_tree->Branch("lucidDigit_tubeId", &m_tubeId);
    m_tree->Branch("lucidDigit_npe"   , &m_npe);
    m_tree->Branch("lucidDigit_npeGas", &m_npeGas);
    m_tree->Branch("lucidDigit_npePmt", &m_npePmt);
    m_tree->Branch("lucidDigit_qdc"   , &m_qdc);
    m_tree->Branch("lucidDigit_tdc"   , &m_tdc);
    m_tree->Branch("lucidDigit_isHit" , &m_isHit);

    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------
  
  StatusCode TruthLucidLvl1NtComponent::fill(void) {
    StatusCode sc;

    // Clear the ntuple vectors
    m_side->clear();
    m_tubeId->clear();
    m_npe->clear();
    m_npeGas->clear();
    m_npePmt->clear();
    m_qdc->clear();
    m_tdc->clear();
    m_isHit->clear();
    m_ntube=-999;
    const LUCID_DigitContainer* lucid_DigitContainer = 0;
    
    sc=(*m_evtStore)->retrieve(lucid_DigitContainer, m_lucid_DigitContainer_key);
    if(sc.isFailure() || !lucid_DigitContainer) {
      if (!m_warningPrinted) {
	(*m_msg) << MSG::WARNING <<   m_lucid_DigitContainer_key <<  " requested but not found." << endreq;
	m_warningPrinted = true;
      }
      return StatusCode::SUCCESS;
    }
    
   
    // Loop over the LUCID_DigitContainer
    LUCID_DigitContainer::const_iterator itr=lucid_DigitContainer->begin();
    LUCID_DigitContainer::const_iterator itr_end=lucid_DigitContainer->end();
    m_ntube=0;
    for(; itr != itr_end; ++itr) {
      if((*itr)->isSideC() && (*itr)->isSideA()) {
	m_side->push_back(2); // Should never happen.
      }
      else if((*itr)->isSideC()) {
	m_side->push_back(-1);
      }
      else if((*itr)->isSideA()) {
	m_side->push_back(1);
      }
      else {
	m_side->push_back(0); // Should never happen.
      }
      m_tubeId->push_back((*itr)->getTubeID());
      m_npe->push_back((*itr)->getNpe());
      m_npeGas->push_back((*itr)->getNpeGas());
      m_npePmt->push_back((*itr)->getNpePmt());
      m_qdc->push_back((*itr)->getQDC());
      m_tdc->push_back((*itr)->getTDC());
      m_isHit->push_back((*itr)->isHit());

      m_ntube++;
    }
 
    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------
}
