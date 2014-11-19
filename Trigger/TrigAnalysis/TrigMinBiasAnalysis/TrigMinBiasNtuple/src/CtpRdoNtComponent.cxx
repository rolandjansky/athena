/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMinBiasNtuple/CtpRdoNtComponent.h"
#include "TrigNtupleBase/NtupleAlgorithm.h"

#include "TrigT1Result/CTP_RDO.h"

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/MsgStream.h"

namespace NtComponent {
  
  //---------------------------------------------------------------------

  CtpRdoNtComponent::CtpRdoNtComponent(NtupleAlgorithm *mainAlg,
				       NtComponentParameters parameters):
    NtupleComponent(mainAlg, parameters) {
  }
  
  //---------------------------------------------------------------------

  CtpRdoNtComponent::~CtpRdoNtComponent() {
  }

  //---------------------------------------------------------------------

  StatusCode CtpRdoNtComponent::book(void) {

    // Set the key from python or use the default.
    std::map<std::string,std::string>::iterator itr;
    itr = m_parameters.containerKeys.find("CTP_RDO"); // Search for StoreGate class.
    if(itr == m_parameters.containerKeys.end()) {
      m_ctp_RdoKey = "CTP_RDO"; // The default when no key is found.
    }
    else {
      m_ctp_RdoKey = (*itr).second; // Use the setting.
    }

    // Create new objects but do not delete them such that they can be
    // stored in the TTree
    m_pit = new std::vector<uint32_t>();
    m_tbp = new std::vector<uint32_t>();
    m_tap = new std::vector<uint32_t>();
    m_tav = new std::vector<uint32_t>();

    m_tree->Branch("lvl1_ctprdo_nBC", &m_nBC, "lvl1_ctprdo_nBC/i");
    m_tree->Branch("lvl1_ctprdo_lvl1aBC", &m_L1A_BC, "lvl1_ctprdo_lvl1aBC/i");
    m_tree->Branch("lvl1_ctprdo_time_s" ,&m_timeSec, "lvl1_ctprdo_time_s/i");
    m_tree->Branch("lvl1_ctprdo_time_ns", &m_timeNanoSec, "lvl1_ctprdo_time_ns/i");
    m_tree->Branch("lvl1_ctprdo_pit", &m_pit);
    m_tree->Branch("lvl1_ctprdo_tbp", &m_tbp);
    m_tree->Branch("lvl1_ctprdo_tap", &m_tap);
    m_tree->Branch("lvl1_ctprdo_tav", &m_tav);

    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------
  
  StatusCode CtpRdoNtComponent::fill(void) {
    StatusCode sc;

    const CTP_RDO *ctpRDO = 0;
    
    m_nBC         = 999;
    m_L1A_BC      = 999;
    m_timeSec     = 999; 
    m_timeNanoSec = 999;
    
    // Clear ntuple vectors
    m_pit->clear();
    m_tbp->clear();
    m_tap->clear();
    m_tav->clear();

    sc=(*m_evtStore)->retrieve(ctpRDO, m_ctp_RdoKey);
    if( sc.isFailure()  || !ctpRDO ) {
      if (!m_warningPrinted) {
	(*m_msg) << MSG::WARNING << m_ctp_RdoKey << " requested but not found." << endreq;
	m_warningPrinted = true;
      }
      return StatusCode::SUCCESS;      
    }
    
    m_nBC = ctpRDO->getNumberOfBunches();
    m_L1A_BC = ctpRDO->getL1AcceptBunchPosition();
    m_timeSec = ctpRDO->getTimeSec();
    m_timeNanoSec = ctpRDO->getTimeNanoSec();   

    // Get the datawords for all bunches that were read out.
    *m_pit = ctpRDO->getTIPWords(); // this was changed from PIT (Run1 = 160 internal trigger lines) to TIP (Run2 = 320 internal trigger lines + 192 direct inputs from L1Topo + ALFA) // joerg
    *m_tbp = ctpRDO->getTBPWords();
    *m_tap = ctpRDO->getTAPWords();
    *m_tav = ctpRDO->getTAVWords();
    
    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------
}
