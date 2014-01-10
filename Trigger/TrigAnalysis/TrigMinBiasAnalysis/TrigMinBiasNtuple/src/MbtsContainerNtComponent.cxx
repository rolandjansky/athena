/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMinBiasNtuple/MbtsContainerNtComponent.h"
#include "TrigNtupleBase/NtupleAlgorithm.h"

#include "TileIdentifier/TileTBID.h"
#include "TileEvent/TileContainer.h"

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/MsgStream.h"

namespace NtComponent {
  
  //---------------------------------------------------------------------

  MbtsContainerNtComponent::MbtsContainerNtComponent(NtupleAlgorithm *mainAlg,
						     NtComponentParameters parameters):
    NtupleComponent(mainAlg, parameters) {
  }
  
  //---------------------------------------------------------------------

  MbtsContainerNtComponent::~MbtsContainerNtComponent() {
  }

  //---------------------------------------------------------------------
  
  StatusCode MbtsContainerNtComponent::setupServices(void) {
    StatusCode sc;

    // Set the key from python or use the default.
    std::map<std::string,std::string>::iterator itr;
    itr = m_parameters.containerKeys.find("TileCellContainer"); // Search for StoreGate class.
    if(itr == m_parameters.containerKeys.end()) {
      m_mbtsContainerName = "MBTSContainer"; // The default when no key is found.
    }
    else {
      m_mbtsContainerName = (*itr).second; // Use the setting.
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

  StatusCode MbtsContainerNtComponent::book(void) {

    // Create new objects but do not delete them such that they can be
    // stored in the TTree
    m_energy = new std::vector<float>();
    m_time = new std::vector<float>();
    m_quality = new std::vector<float>();
    m_type = new std::vector<int>();
    m_module = new std::vector<int>();
    m_channel = new std::vector<int>();

    m_tree->Branch("mbts_ncell", &m_nchan,"mbts_ncell/I");
    m_tree->Branch("mbts_energy", &m_energy);
    m_tree->Branch("mbts_time", &m_time);
    m_tree->Branch("mbts_quality", &m_quality);
    m_tree->Branch("mbts_type", &m_type);
    m_tree->Branch("mbts_module", &m_module);
    m_tree->Branch("mbts_channel",&m_channel);
    
    return StatusCode::SUCCESS;
  }
  
  //---------------------------------------------------------------------
  
  StatusCode MbtsContainerNtComponent::fill(void) {
    StatusCode sc;
    const TileCellContainer *tileCellCnt = 0;
    
  
    m_nchan=-999;

    // Clear the ntuple vectors
    m_energy->clear();
    m_time->clear();
    m_quality->clear();
    m_type->clear();
    m_module->clear();
    m_channel->clear();


    sc=(*m_evtStore)->retrieve(tileCellCnt, m_mbtsContainerName);
    if( sc.isFailure()  || !tileCellCnt ) {
      if (!m_warningPrinted) {
	(*m_msg) << MSG::WARNING << m_mbtsContainerName <<" requested but not found." << endreq;
	m_warningPrinted = true;
      }
      return StatusCode::SUCCESS;
    }
    
    int count = 0;
    TileCellContainer::const_iterator itr = tileCellCnt->begin();
    TileCellContainer::const_iterator itr_end = tileCellCnt->end();
    for(; itr != itr_end; ++itr) {
      m_energy->push_back((*itr)->energy());
      m_time->push_back((*itr)->time());
      m_quality->push_back((*itr)->quality());
      
      Identifier id=(*itr)->ID();
      m_type->push_back(m_tileTBID->type(id));
      m_channel->push_back(m_tileTBID->channel(id));
      m_module->push_back(m_tileTBID->module(id));
      
      count++;
    }

    m_nchan = count;
    if(m_nchan>32) {
      (*m_msg) << MSG::WARNING << "There shoule be <=32 MBTS TileCells in " << m_mbtsContainerName << ".  " << m_nchan << " were found." << endreq;
    }
    
    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------
}
