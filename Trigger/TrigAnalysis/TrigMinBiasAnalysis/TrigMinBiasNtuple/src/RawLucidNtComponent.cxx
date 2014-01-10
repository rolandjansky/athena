/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMinBiasNtuple/RawLucidNtComponent.h"
#include "TrigNtupleBase/NtupleAlgorithm.h"

#include "LUCID_RawEvent/LUCID_RawDataContainer.h"

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/MsgStream.h"

#include <vector>

namespace NtComponent {
  
  //---------------------------------------------------------------------

  RawLucidNtComponent::RawLucidNtComponent(NtupleAlgorithm *mainAlg,
					   NtComponentParameters parameters):
    NtupleComponent(mainAlg, parameters) {
  }
  
  //---------------------------------------------------------------------

  RawLucidNtComponent::~RawLucidNtComponent() {
  }

  //---------------------------------------------------------------------

  StatusCode RawLucidNtComponent::book(void) {

    // Set the key from python or use the default.
    std::map<std::string,std::string>::iterator itr;
    itr = m_parameters.containerKeys.find("LUCID_RawDataContainer"); // Search for StoreGate class.

    if (itr == m_parameters.containerKeys.end()) {
      m_lucid_RawDataContainer_key = "Lucid_RawData"; // The default when no key is found.
    }
    else {
      m_lucid_RawDataContainer_key = (*itr).second; // Use the setting.
    }
    
    // Create new objects but do not delete them such that they can be stored in the TTree
    
    m_tree->Branch("lucidRaw_word0" , &m_word0 , "lucidRaw_word0/I");
    m_tree->Branch("lucidRaw_word1" , &m_word1 , "lucidRaw_word1/I");
    m_tree->Branch("lucidRaw_word2" , &m_word2 , "lucidRaw_word2/I");
    m_tree->Branch("lucidRaw_word3" , &m_word3 , "lucidRaw_word3/I");

    m_tree->Branch("lucidRaw_word0p", &m_word0p, "lucidRaw_word0p/I");
    m_tree->Branch("lucidRaw_word1p", &m_word1p, "lucidRaw_word1p/I");
    m_tree->Branch("lucidRaw_word2p", &m_word2p, "lucidRaw_word2p/I");
    m_tree->Branch("lucidRaw_word3p", &m_word3p, "lucidRaw_word3p/I");

    m_tree->Branch("lucidRaw_word0n", &m_word0n, "lucidRaw_word0n/I");
    m_tree->Branch("lucidRaw_word1n", &m_word1n, "lucidRaw_word1n/I");
    m_tree->Branch("lucidRaw_word2n", &m_word2n, "lucidRaw_word2n/I");
    m_tree->Branch("lucidRaw_word3n", &m_word3n, "lucidRaw_word3n/I");

    m_tree->Branch("lucidRaw_status", &m_status, "lucidRaw_status/I");


    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------
  
  StatusCode RawLucidNtComponent::fill(void) {

    StatusCode sc;

    // Clear the ntuple vectors
    m_word0  = -999;
    m_word1  = -999;
    m_word2  = -999;
    m_word3  = -999;

    m_word0p = -999;
    m_word1p = -999;
    m_word2p = -999;
    m_word3p = -999;

    m_word0n = -999;
    m_word1n = -999;
    m_word2n = -999;
    m_word3n = -999;

    m_status = -999;

    const LUCID_RawDataContainer* lucid_RawDataContainer = 0;
    
    sc=(*m_evtStore)->retrieve(lucid_RawDataContainer, m_lucid_RawDataContainer_key);

    if (sc.isFailure() || !lucid_RawDataContainer) {

      if (!m_warningPrinted) {
	(*m_msg) << MSG::WARNING <<   m_lucid_RawDataContainer_key <<  " requested but not found." << endreq;
	m_warningPrinted = true;
      }
      return StatusCode::SUCCESS;
    }
   
    // Loop over the LUCID_RawDataContainer
    LUCID_RawDataContainer::const_iterator itr     = lucid_RawDataContainer->begin();
    LUCID_RawDataContainer::const_iterator itr_end = lucid_RawDataContainer->end();

    int count = 0;
    for(; itr != itr_end; ++itr) {

      m_word0  = (*itr)->getWord0();
      m_word1  = (*itr)->getWord1();
      m_word2  = (*itr)->getWord2();
      m_word3  = (*itr)->getWord3();

      m_word0p = (*itr)->getWord0p();
      m_word1p = (*itr)->getWord1p();
      m_word2p = (*itr)->getWord2p();
      m_word3p = (*itr)->getWord3p();

      m_word0n = (*itr)->getWord0n();
      m_word1n = (*itr)->getWord1n();
      m_word2n = (*itr)->getWord2n();
      m_word3n = (*itr)->getWord3n();

      m_status = (*itr)->getStatus();
      
      count++;
    }
    
    if (count > 1) (*m_msg) << MSG::WARNING << count << " LUCID_RawData objects found, only 1 expected." << endreq;
    
    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------
}
