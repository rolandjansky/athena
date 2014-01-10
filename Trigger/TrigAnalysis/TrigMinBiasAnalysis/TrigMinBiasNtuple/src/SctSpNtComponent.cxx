/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMinBiasNtuple/SctSpNtComponent.h"
#include "TrigNtupleBase/NtupleAlgorithm.h"

#include "TrkSpacePoint/SpacePointContainer.h"

#include "InDetIdentifier/SCT_ID.h"

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/MsgStream.h"

namespace NtComponent {
  
  //---------------------------------------------------------------------

  SctSpNtComponent::SctSpNtComponent(NtupleAlgorithm *mainAlg,
				     NtComponentParameters parameters):
    NtupleComponent(mainAlg, parameters) {
  }
  
  //---------------------------------------------------------------------

  SctSpNtComponent::~SctSpNtComponent() {
  }

  //---------------------------------------------------------------------

  StatusCode SctSpNtComponent::setupServices(void) {
    StatusCode sc;
    
    if((sc = NtupleComponent::setupServices()).isFailure()) return sc;
    
    // Get a SCT identifier helper
    sc = (*m_detStore)->retrieve(m_sctHelper, "SCT_ID");
    if(sc.isFailure()) {
      (*m_msg) << MSG::ERROR << "Can't get " << m_sctHelper << endreq;
      return StatusCode::FAILURE;
    }

    // Set the key from python or use the default.
    std::map<std::string,std::string>::iterator itr;
    itr = m_parameters.containerKeys.find("SpacePointContainer"); // Search for StoreGate class.
    if(itr == m_parameters.containerKeys.end()) {
      m_sctSpacePointsKey = "SCT_SpacePoints"; // The default when no key is found.
    }
    else {
      m_sctSpacePointsKey = (*itr).second; // Use the setting.
    }
    
    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------

  StatusCode SctSpNtComponent::book(void) {

    // Create new objects but do not delete them such that they can be
    // stored in the TTree
    m_sp_barrel_ec     = new std::vector<short>();
    m_sp_layer_disk    = new std::vector<short>();
    m_sp_phi_module    = new std::vector<short>();
    m_sp_eta_module    = new std::vector<short>();
    m_sp_hashid_module = new std::vector<int>();
    m_sp_global_x      = new std::vector<float>();
    m_sp_global_y      = new std::vector<float>();
    m_sp_global_z      = new std::vector<float>();
    
    m_tree->Branch("sct_nsp",              &m_nsp, "nsct_nsp/I");
    m_tree->Branch("sct_sp_barrel_ec",     &m_sp_barrel_ec);
    m_tree->Branch("sct_sp_layer_disk",    &m_sp_layer_disk);
    m_tree->Branch("sct_sp_phi_module",    &m_sp_phi_module);
    m_tree->Branch("sct_sp_eta_module",    &m_sp_eta_module);
    m_tree->Branch("sct_sp_hashid_module", &m_sp_hashid_module);
    m_tree->Branch("sct_sp_global_x",      &m_sp_global_x);
    m_tree->Branch("sct_sp_global_y",      &m_sp_global_y);
    m_tree->Branch("sct_sp_global_z",      &m_sp_global_z);
    
    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------
  
  StatusCode SctSpNtComponent::fill(void) {
   
    // Clear ntuple variables
    m_nsp = -9;
    m_sp_barrel_ec->clear();
    m_sp_layer_disk->clear();
    m_sp_phi_module->clear();
    m_sp_eta_module->clear();
    m_sp_hashid_module->clear();
    m_sp_global_x->clear();
    m_sp_global_y->clear();
    m_sp_global_z->clear();

    StatusCode sc; 
    
    const SpacePointContainer *sctSpCont=0;   
    sc = (*m_evtStore)->retrieve( sctSpCont, m_sctSpacePointsKey );
    if( sc.isFailure() || !sctSpCont) {
      if (!m_warningPrinted) {
	(*m_msg) << MSG::WARNING <<  m_sctSpacePointsKey <<  " requested but not found." << endreq;
	m_warningPrinted = true;
      }
      return StatusCode::SUCCESS;
    }

    m_nsp = 0;
    // Loop over the space point container
    SpacePointContainer::const_iterator itr = sctSpCont->begin();
    SpacePointContainer::const_iterator itr_end = sctSpCont->end();     
    for(; itr!=itr_end; ++itr){

      // Find out which module this is
      Identifier id = (*itr)->identify();

      // Cache the values to flatten the data structure.
      short barrel_ec  = m_sctHelper->barrel_ec(id);
      short layer_disk = m_sctHelper->layer_disk(id);
      short phi_module = m_sctHelper->phi_module(id);
      short eta_module = m_sctHelper->eta_module(id);
      int   hash_id    = m_sctHelper->wafer_hash(id);

      // Loop over the collection: one per module, if module has space points?
      SpacePointCollection::const_iterator itr_spCol = (*itr)->begin();
      SpacePointCollection::const_iterator itr_spCol_end = (*itr)->end();

      for(; itr_spCol!=itr_spCol_end; ++itr_spCol) {
	m_sp_barrel_ec->push_back(barrel_ec);
	m_sp_layer_disk->push_back(layer_disk);
	m_sp_phi_module->push_back(phi_module);
	m_sp_eta_module->push_back(eta_module);
	m_sp_hashid_module->push_back(hash_id);
	m_sp_global_x->push_back((*itr_spCol)->globalPosition().x());
	m_sp_global_y->push_back((*itr_spCol)->globalPosition().y());
	m_sp_global_z->push_back((*itr_spCol)->globalPosition().z());

	// Increment the number of space points
	m_nsp++;
      }
    }

    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------
}
