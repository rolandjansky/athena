/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMinBiasNtuple/CaloCellNtComponent.h"
#include "TrigNtupleBase/NtupleAlgorithm.h"

#include "CaloIdentifier/TileID.h"
#include "CaloEvent/CaloCell.h"
#include "CaloEvent/CaloCellContainer.h"

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/MsgStream.h"

namespace NtComponent {

  //---------------------------------------------------------------------
  
  CaloCellNtComponent::CaloCellNtComponent(NtupleAlgorithm *mainAlg,
					   NtComponentParameters parameters):
    NtupleComponent(mainAlg, parameters) {
  }
  
  //---------------------------------------------------------------------
  
  CaloCellNtComponent::~CaloCellNtComponent() {
  }
  
  //---------------------------------------------------------------------
  
  StatusCode CaloCellNtComponent::setupServices(void) {
    StatusCode sc;
    
    //m_CaloCellContName = m_mainAlg->caloCellContainerKey();
    //m_useTile = m_mainAlg->useTileCell();
    //m_useLarg = m_mainAlg->useLargCell();
    //m_caloDetail = m_mainAlg->caloDetail();

    (*m_msg) << MSG::ERROR << "This ntuple component needs to be broken up." << endreq;
    return StatusCode::FAILURE;

    // Initialise base class services
    if((sc = NtupleComponent::setupServices()).isFailure()) return sc;
    
    // Get the test beam identifier for the MBTS
    sc = (*m_detStore)->retrieve(m_tileID);
    if (sc.isFailure()) {
      (*m_msg) << MSG::ERROR
	       << "Unable to retrieve TileID helper from DetectorStore" << endreq;
      return sc;
    }
    
    
    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------
  
  StatusCode CaloCellNtComponent::book(void) {
    StatusCode sc;
    
    // Create new objects but do not delete them such that they can be
    // stored in the TTree
    m_energy = new std::vector<float>();
    m_time = new std::vector<float>();
    m_eta = new std::vector<float>();
    m_phi = new std::vector<float>();
    m_quality = new std::vector<float>();
    m_tree->Branch("calo_ncell",   &m_ncell,"calo_ncell/I");
    m_tree->Branch("calo_energy",  &m_energy);
    m_tree->Branch("calo_time",    &m_time);
    m_tree->Branch("calo_eta",     &m_eta);
    m_tree->Branch("calo_phi",     &m_phi);
    m_tree->Branch("calo_quality", &m_quality);
    if(m_caloDetail > 0) {
      m_type = new std::vector<int>();
      m_side = new std::vector<int>();
      m_module = new std::vector<int>();
      m_tower = new std::vector<int>();
      m_sample = new std::vector<int>();
      m_tree->Branch("calo_type",    &m_type);
      m_tree->Branch("calo_side",    &m_side);
      m_tree->Branch("calo_module",  &m_module);
      m_tree->Branch("calo_tower",   &m_tower);
      m_tree->Branch("calo_sample",  &m_sample);
      if(m_caloDetail > 1) {
	m_x = new std::vector<float>();
	m_y = new std::vector<float>();
	m_z = new std::vector<float>();
	m_tree->Branch("calo_x",       &m_x);
	m_tree->Branch("calo_y",       &m_y);
	m_tree->Branch("calo_z",       &m_z);
      }
    }
    
    return StatusCode::SUCCESS;
  }
  
  //---------------------------------------------------------------------
  
  StatusCode CaloCellNtComponent::fill(void) {
    StatusCode sc;
    const CaloCellContainer *caloCellCnt = 0;
    sc=(*m_evtStore)->retrieve(caloCellCnt, m_CaloCellContName);
    if( sc.isFailure()  || !caloCellCnt ) {
      (*m_msg) << MSG::ERROR << "Error retrieving " << m_CaloCellContName << endreq;
      return StatusCode::FAILURE;
    }
    
    m_ncell=0;
    
    // Clear the ntuple vectors
    m_energy->clear();
    m_time->clear();
    m_eta->clear();
    m_phi->clear();
    m_quality->clear();
    if(m_caloDetail > 0) {
      m_type->clear();
      m_side->clear();
      m_module->clear();
      m_tower->clear();
      m_sample->clear();
      if(m_caloDetail > 1) {
	m_x->clear();
	m_y->clear();
	m_z->clear();
      }
    }
    
    CaloCellContainer::const_iterator itr = caloCellCnt->begin();
    CaloCellContainer::const_iterator itr_end = caloCellCnt->end();
    for(; itr != itr_end; ++itr) {
      Identifier id=(*itr)->ID();
      if( (m_useTile && m_tileID->is_tile(id)) || (m_useLarg && !(m_tileID->is_tile(id))) ) {
	m_energy->push_back((*itr)->energy());
	m_time->push_back((*itr)->time());
	m_eta->push_back((*itr)->eta());
	m_phi->push_back((*itr)->phi());
	m_quality->push_back((*itr)->quality());
	
	if(m_caloDetail > 0) {
	  if(m_tileID->is_tile(id)) {
	    m_type->push_back(1);
	    m_side->push_back(m_tileID->side(id));
	    m_module->push_back(m_tileID->module(id));
	    m_tower->push_back(m_tileID->tower(id));
	    m_sample->push_back(m_tileID->sample(id));
	  }
	  else {
	    m_type->push_back(0);
	    m_side->push_back(0);
	    m_module->push_back(-1);
	    m_tower->push_back(-1);
	    m_sample->push_back(-1);
	  }
	  if(m_caloDetail > 1) {
	    m_x->push_back((*itr)->x());
	    m_y->push_back((*itr)->y());
	    m_z->push_back((*itr)->z());
	  }
	}
	m_ncell++;
      }
    }
    
    return StatusCode::SUCCESS;
  }
  
  //---------------------------------------------------------------------
}
