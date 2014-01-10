/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMinBiasNtuple/TrtLvl2NtComponent.h"
#include "TrigNtupleBase/NtupleAlgorithm.h"

#include "TrigInDetEvent/TrigTrtHitCountsCollection.h"

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/MsgStream.h"

namespace NtComponent {
  
  //---------------------------------------------------------------------

  TrtLvl2NtComponent::TrtLvl2NtComponent(NtupleAlgorithm *mainAlg,
					 NtComponentParameters parameters):
    NtupleComponent(mainAlg, parameters) {
  }
  
  //---------------------------------------------------------------------

  TrtLvl2NtComponent::~TrtLvl2NtComponent() {
  }

  //---------------------------------------------------------------------

  StatusCode TrtLvl2NtComponent::book(void) {

    // Set the key from python or use the default.
    std::map<std::string,std::string>::iterator itr;
    itr = m_parameters.containerKeys.find("TrigTrtHitCountsCollection"); // Search for StoreGate class.
    if(itr == m_parameters.containerKeys.end()) {
      m_trigTrtHitCountsCollectionKey = "HLT_TrtHitCount"; // The default when no key is found.
    }
    else {
      m_trigTrtHitCountsCollectionKey = (*itr).second; // Use the setting.
    }

    // Create new objects but do not delete them such that they can be
    // stored in the TTree
    m_endcapC_contents = new std::vector<float>();
    m_barrel_contents = new std::vector<float>();
    m_endcapA_contents = new std::vector<float>();

    m_tree->Branch("lvl2_mbTrt_nElements", &m_nElements, "lvl2_mbTrt_nElements/i");
    m_tree->Branch("lvl2_mbTrt_hTotBins", &m_hTotBins, "lvl2_mbTrt_hTotBins/i");
    m_tree->Branch("lvl2_mbTrt_hTotMin", &m_hTotMin, "lvl2_mbTrt_hTotMin/F");
    m_tree->Branch("lvl2_mbTrt_hTotMax", &m_hTotMax, "lvl2_mbTrt_hTotMax/F");
    m_tree->Branch("lvl2_mbTrt_endcapC_contents", &m_endcapC_contents);
    m_tree->Branch("lvl2_mbTrt_barrel_contents", &m_barrel_contents);
    m_tree->Branch("lvl2_mbTrt_endcapA_contents", &m_endcapA_contents);

    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------
  
  StatusCode TrtLvl2NtComponent::fill(void) {
    StatusCode sc;
    const TrigTrtHitCountsCollection *trigTrtHitCountsCollection = 0;

    // Clear ntuple variables
    m_nElements = 0;
    m_hTotBins = 0;
    m_hTotMin = 0;
    m_hTotMax = 0;
    m_endcapC_contents->clear();
    m_barrel_contents->clear();
    m_endcapA_contents->clear();

    // Container is present only if the level 1 trigger item that
    // seeds the level 2 trigger item has fired.
    sc=(*m_evtStore)->retrieve(trigTrtHitCountsCollection, m_trigTrtHitCountsCollectionKey);
    if(sc.isFailure()  || !trigTrtHitCountsCollection) {
      if (!m_warningPrinted) {
	(*m_msg) << MSG::WARNING <<  m_trigTrtHitCountsCollectionKey  <<  " requested but not found." << endreq;
	m_warningPrinted = true;
      }
      return StatusCode::SUCCESS;
    }
    
    TrigHisto1D endcapC;
    TrigHisto1D barrel;
    TrigHisto1D endcapA;

    m_nElements = trigTrtHitCountsCollection->size();
    (*m_msg) << MSG::DEBUG << "There are " << m_nElements << " TrigTrtHitCounts entries." << endreq;   
    
    TrigTrtHitCountsCollection::const_iterator itr = trigTrtHitCountsCollection->begin();
    TrigTrtHitCountsCollection::const_iterator itr_end = trigTrtHitCountsCollection->end();

    // The TE is full scan and not connected to an RoI.  All TEs
    // within a given event are therefore the same.  Just get the first TE.
    if(itr != itr_end) {
      (*m_msg) << MSG::DEBUG << "Dumping the first TrigTrtHitCounts entry." << endreq;

      // Get a copy of the TrigHisto objects
      endcapC = (*itr)->endcapC();
      barrel = (*itr)->barrel();
      endcapA = (*itr)->endcapA();

      // The TrigHisto parameters are the same for endcapC, barrel and endcapA
      m_hTotBins = endcapC.nbins_x();
      m_hTotMin = endcapC.min_x();
      m_hTotMax = endcapC.max_x();
      *m_endcapC_contents = endcapC.contents();
      *m_barrel_contents = barrel.contents();
      *m_endcapA_contents = endcapA.contents();
    }
    
    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------
}
