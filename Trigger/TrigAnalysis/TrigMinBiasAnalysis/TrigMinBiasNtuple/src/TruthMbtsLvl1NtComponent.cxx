/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMinBiasNtuple/TruthMbtsLvl1NtComponent.h"
#include "TrigNtupleBase/NtupleAlgorithm.h"

#include "TileIdentifier/TileTBID.h"
#include "TileEvent/TileContainer.h"

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/MsgStream.h"

#include <vector>

namespace NtComponent {
  
  //---------------------------------------------------------------------

  TruthMbtsLvl1NtComponent::TruthMbtsLvl1NtComponent(NtupleAlgorithm *mainAlg,
						     NtComponentParameters parameters):
    NtupleComponent(mainAlg, parameters) {
  }
  
  //---------------------------------------------------------------------

  TruthMbtsLvl1NtComponent::~TruthMbtsLvl1NtComponent() {
  }

  //---------------------------------------------------------------------

  StatusCode TruthMbtsLvl1NtComponent::setupServices(void) {
    StatusCode sc;

    // Set the key from python or use the default.
    std::map<std::string,std::string>::iterator itr;
    itr = m_parameters.containerKeys.find("TileTTL1Container"); // Search for StoreGate class.
    if(itr == m_parameters.containerKeys.end()) {
      m_tileTTL1ContainerKey = "TileTTL1MBTS"; // The default when no key is found.
    }
    else {
      m_tileTTL1ContainerKey = (*itr).second; // Use the setting.
    }

    // Initialise base class services
    if((sc = NtupleComponent::setupServices()).isFailure()) return sc;

    // Get the test beam identifier for the MBTS
    sc = (*m_detStore)->retrieve(m_tileTBID);
    if (sc.isFailure()) {
      (*m_msg) << MSG::ERROR
	       << "Unable to retrieve TileTBID helper from DetectorStore" << endreq;
      return sc;
    }

    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------

  StatusCode TruthMbtsLvl1NtComponent::book(void) {

    // Create new objects but do not delete them such that they can be
    // stored in the TTree
    m_ttl1_side = new std::vector<int>();
    m_ttl1_eta = new std::vector<int>();
    m_ttl1_phi = new std::vector<int>();
    m_ttl1_samples = new std::vector<std::vector<double> >();

    m_tree->Branch("tileTTL1_nttl1",   &m_ttl1_nchan, "tileTTL1_nttl1/I");
    m_tree->Branch("tileTTL1_side",    &m_ttl1_side);
    m_tree->Branch("tileTTL1_eta",     &m_ttl1_eta);
    m_tree->Branch("tileTTL1_phi",     &m_ttl1_phi);
    m_tree->Branch("tileTTL1_samples", &m_ttl1_samples);

    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------
  
  StatusCode TruthMbtsLvl1NtComponent::fill(void) {
    StatusCode sc;


  // Clear the ntuple vectors
    m_ttl1_nchan = -999;
    m_ttl1_side->clear();
    m_ttl1_eta->clear();
    m_ttl1_phi->clear();
    m_ttl1_samples->clear();

    const TileTTL1Container* tileTTL1MBTS = 0;

    sc=(*m_evtStore)->retrieve(tileTTL1MBTS, m_tileTTL1ContainerKey);
    if(sc.isFailure() || !tileTTL1MBTS) {
      if (!m_warningPrinted) {
	(*m_msg) << MSG::WARNING << m_tileTTL1ContainerKey  <<  " requested but not found." << endreq;
	m_warningPrinted = true;
      }
      return StatusCode::SUCCESS;
    }
   
  
    // Loop over the TileTTL1MBTS container
    TileTTL1Container::const_iterator it=tileTTL1MBTS->begin();
    TileTTL1Container::const_iterator end=tileTTL1MBTS->end();
  
    m_ttl1_nchan=0;
    for(; it != end; ++it) {
      const TileTTL1 * cinp = (*it);
      Identifier id=cinp->TTL1_ID();

      if (m_tileTBID->is_tiletb(id)) {
        m_ttl1_side->push_back(m_tileTBID->type(id));
        m_ttl1_eta->push_back(m_tileTBID->channel(id));
        m_ttl1_phi->push_back(m_tileTBID->module(id));
      } 
      else {
        (*m_msg) << MSG::ERROR << "This is not an TileTTL1MBTS identifier!" << endreq;
        return StatusCode::FAILURE;
      }
    
      //      std::vector <double> samples = cinp->samples();
      m_ttl1_samples->push_back(cinp->samples());

      m_ttl1_nchan++;
    }
 
    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------
}
