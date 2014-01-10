/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMinBiasNtuple/BcmRdoNtComponent.h"
#include "TrigNtupleBase/NtupleAlgorithm.h"

#include "InDetBCM_RawData/BCM_RDO_Container.h"

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/MsgStream.h"

namespace NtComponent {
  
  //---------------------------------------------------------------------

  BcmRdoNtComponent::BcmRdoNtComponent(NtupleAlgorithm *mainAlg,
				       NtComponentParameters parameters):
    NtupleComponent(mainAlg, parameters) {
  }
  
  //---------------------------------------------------------------------

  BcmRdoNtComponent::~BcmRdoNtComponent() {
  }

  //---------------------------------------------------------------------

  StatusCode BcmRdoNtComponent::book(void) {

    // Set the key from python or use the default.
    std::map<std::string,std::string>::iterator itr;
    itr = m_parameters.containerKeys.find("BCM_RDO_Container"); // Search for StoreGate class.
    if(itr == m_parameters.containerKeys.end()) {
      m_bcmRdoContainerKey = "BCM_RDOs"; // The default when no key is found.
    }
    else {
      m_bcmRdoContainerKey = (*itr).second; // Use the setting.
    }

    // Create new objects but do not delete them such that they can be
    // stored in the TTree
    m_bcm_channel = new std::vector<int>();
    m_bcm_nhits = new std::vector<int>();
    m_bcm_pulse1pos = new std::vector<std::vector<int> >();
    m_bcm_pulse1width = new std::vector<std::vector<int> >();
    m_bcm_pulse2pos = new std::vector<std::vector<int> >();
    m_bcm_pulse2width = new std::vector<std::vector<int> >();
    m_bcm_lvl1a = new std::vector<std::vector<int> >();
    m_bcm_bcid = new std::vector<std::vector<int> >();
    m_bcm_lvl1id = new std::vector<std::vector<int> >();

    m_tree->Branch("bcm_nbcm", &m_bcm_nbcm, "bcm_nbcm/I");
    m_tree->Branch("bcm_channel", &m_bcm_channel);
    m_tree->Branch("bcm_nhits", &m_bcm_nhits);
    m_tree->Branch("bcm_pulse1pos", &m_bcm_pulse1pos);
    m_tree->Branch("bcm_pulse1width", &m_bcm_pulse1width);
    m_tree->Branch("bcm_pulse2pos", &m_bcm_pulse2pos);
    m_tree->Branch("bcm_pulse2width", &m_bcm_pulse2width);
    m_tree->Branch("bcm_lvl1a", &m_bcm_lvl1a);
    m_tree->Branch("bcm_bcid", &m_bcm_bcid);
    m_tree->Branch("bcm_lvl1id", &m_bcm_lvl1id);
    
    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------
  
  StatusCode BcmRdoNtComponent::fill(void) {
    StatusCode sc;
    const BCM_RDO_Container *bcmRdo = 0;

    m_bcm_nbcm = -999;
    // Clear ntuple vectors
    m_bcm_channel->clear();
    m_bcm_nhits->clear();
    m_bcm_pulse1pos->clear();
    m_bcm_pulse1width->clear();
    m_bcm_pulse2pos->clear();
    m_bcm_pulse2width->clear();
    m_bcm_lvl1a->clear();
    m_bcm_bcid->clear();
    m_bcm_lvl1id->clear();

    sc=(*m_evtStore)->retrieve(bcmRdo, m_bcmRdoContainerKey);
    if( sc.isFailure()  || !bcmRdo ) {
      if (!m_warningPrinted) {
	(*m_msg) << MSG::WARNING << m_bcmRdoContainerKey << " requested but not found." << endreq;
        m_warningPrinted = true;
      }
      return StatusCode::SUCCESS;
    }

    BCM_RDO_Container::const_iterator itr = bcmRdo->begin();
    BCM_RDO_Container::const_iterator itr_end = bcmRdo->end();
    int ihit, ibcm = 0;
    for(;itr != itr_end; ++itr) {
      BCM_RDO_Collection::const_iterator hit_itr = (*itr)->begin();
      BCM_RDO_Collection::const_iterator hit_itr_end = (*itr)->end();
      ihit = 0;
      std::vector<int> pulse1pos;
      std::vector<int> pulse1width;
      std::vector<int> pulse2pos;
      std::vector<int> pulse2width;
      std::vector<int> lvl1a;
      std::vector<int> bcid;
      std::vector<int> lvl1id;
      for(;hit_itr != hit_itr_end; ++hit_itr) {
	pulse1pos.push_back((*hit_itr)->getPulse1Position());
	pulse1width.push_back((*hit_itr)->getPulse1Width());
	pulse2pos.push_back((*hit_itr)->getPulse2Position());
	pulse2width.push_back((*hit_itr)->getPulse2Width());
	lvl1a.push_back((*hit_itr)->getLVL1A());
	bcid.push_back((*hit_itr)->getBCID());
	lvl1id.push_back((*hit_itr)->getLVL1ID());
        
        ihit++;
      }

      m_bcm_channel->push_back((*itr)->getChannel());
      m_bcm_nhits->push_back(ihit);
      m_bcm_pulse1pos->push_back(pulse1pos);
      m_bcm_pulse1width->push_back(pulse1width);
      m_bcm_pulse2pos->push_back(pulse2pos);
      m_bcm_pulse2width->push_back(pulse2width);
      m_bcm_lvl1a->push_back(lvl1a);
      m_bcm_bcid->push_back(bcid);
      m_bcm_lvl1id->push_back(lvl1id);

      ibcm++;
    }
    m_bcm_nbcm = ibcm;

    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------
}
