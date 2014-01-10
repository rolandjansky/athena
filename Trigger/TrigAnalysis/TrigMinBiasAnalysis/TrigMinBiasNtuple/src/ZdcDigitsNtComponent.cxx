/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMinBiasNtuple/ZdcDigitsNtComponent.h"
#include "TrigNtupleBase/NtupleAlgorithm.h"

#include "ZdcEvent/ZdcDigitsCollection.h"
//#include "ZdcByteStream/ZdcDefs.h"

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/MsgStream.h"

namespace NtComponent {
  
  //---------------------------------------------------------------------

  ZdcDigitsNtComponent::ZdcDigitsNtComponent(NtupleAlgorithm *mainAlg,
					     NtComponentParameters parameters):
    NtupleComponent(mainAlg, parameters) {
  }
  
  //---------------------------------------------------------------------

  ZdcDigitsNtComponent::~ZdcDigitsNtComponent() {
  }

  //---------------------------------------------------------------------

  StatusCode ZdcDigitsNtComponent::book(void) {

    // Set the key from python or use the default.
    std::map<std::string,std::string>::iterator itr;
    itr = m_parameters.containerKeys.find("ZdcDigitsCollection"); // Search for StoreGate class.
    if(itr == m_parameters.containerKeys.end()) {


      //      m_zdcDigitsCollectionKey = ZdcDefs::ZdcDigitsLocation; // The default when no key is found.
      m_zdcDigitsCollectionKey = "ZdcDigitsCollection";
    }
    else {
      m_zdcDigitsCollectionKey = (*itr).second; // Use the setting.
    }

    // Create new objects but do not delete them such that they can be
    // stored in the TTree
    m_digits_gain0_delay0 = new std::vector<std::vector<int> >();
    m_digits_gain0_delay1 = new std::vector<std::vector<int> >();
    m_digits_gain1_delay0 = new std::vector<std::vector<int> >();
    m_digits_gain1_delay1 = new std::vector<std::vector<int> >();
    
    m_tree->Branch("zdcDigits_gain0_delay0", &m_digits_gain0_delay0);
    m_tree->Branch("zdcDigits_gain0_delay1", &m_digits_gain0_delay1);
    m_tree->Branch("zdcDigits_gain1_delay0", &m_digits_gain1_delay0);
    m_tree->Branch("zdcDigits_gain1_delay1", &m_digits_gain1_delay1);

    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------
  
  StatusCode ZdcDigitsNtComponent::fill(void) {
    StatusCode sc;

    // Clear the ntuple variables
    m_digits_gain0_delay0->clear();
    m_digits_gain0_delay1->clear();
    m_digits_gain1_delay0->clear();
    m_digits_gain1_delay1->clear();

    const ZdcDigitsCollection *zdcDigitsCollection = 0;
    sc = (*m_evtStore)->retrieve(zdcDigitsCollection, m_zdcDigitsCollectionKey);

    if (sc.isFailure() || !zdcDigitsCollection) {
      if (!m_warningPrinted) {
	(*m_msg) << MSG::WARNING << m_zdcDigitsCollectionKey <<  " requested but not found." << endreq;
	m_warningPrinted = true;
      }
      return StatusCode::SUCCESS;
    }

    // Loop over the digits collection.
    ZdcDigitsCollection::const_iterator itr = zdcDigitsCollection->begin();
    ZdcDigitsCollection::const_iterator itr_end = zdcDigitsCollection->end();
    for(;itr!=itr_end;++itr) {
      const std::vector<int>& digits_gain0_delay0 = (*itr)->get_digits_gain0_delay0(); 
      m_digits_gain0_delay0->push_back(digits_gain0_delay0);
      
      const std::vector<int>& digits_gain0_delay1 = (*itr)->get_digits_gain0_delay1(); 
      m_digits_gain0_delay1->push_back(digits_gain0_delay1);

      const std::vector<int>& digits_gain1_delay0 = (*itr)->get_digits_gain1_delay0(); 
      m_digits_gain1_delay0->push_back(digits_gain1_delay0);
      
      const std::vector<int>& digits_gain1_delay1 = (*itr)->get_digits_gain1_delay1(); 
      m_digits_gain1_delay1->push_back(digits_gain1_delay1);
    }
    
    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------
}
