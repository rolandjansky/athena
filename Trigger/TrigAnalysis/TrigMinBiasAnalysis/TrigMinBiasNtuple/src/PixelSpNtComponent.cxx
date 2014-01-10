/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMinBiasNtuple/PixelSpNtComponent.h"
#include "TrigNtupleBase/NtupleAlgorithm.h"

#include "TrkSpacePoint/SpacePointContainer.h"

#include "InDetIdentifier/PixelID.h"

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/MsgStream.h"

namespace NtComponent {
  
  //---------------------------------------------------------------------

  PixelSpNtComponent::PixelSpNtComponent(NtupleAlgorithm *mainAlg,
					     NtComponentParameters parameters):
    NtupleComponent(mainAlg, parameters) {
  }
  
  //---------------------------------------------------------------------

  PixelSpNtComponent::~PixelSpNtComponent() {
  }

  //---------------------------------------------------------------------

  StatusCode PixelSpNtComponent::setupServices(void) {
    StatusCode sc;

    if((sc = NtupleComponent::setupServices()).isFailure()) return sc;
    
    // Get a Pixel identifier helper
    sc = (*m_detStore)->retrieve(m_pixelHelper, "PixelID");
    if(sc.isFailure()) {
      (*m_msg) << MSG::ERROR << "Can not get PixelID helper" << endreq;
      return StatusCode::FAILURE;
    }
    
    // Set the key from python or use the default.
    std::map<std::string,std::string>::iterator itr;
    itr = m_parameters.containerKeys.find("PixelSpacePointContainer"); // Search for StoreGate class.
    if(itr == m_parameters.containerKeys.end()) {
      m_pixelSpacePointsKey = "PixelSpacePoints"; // The default when no key is found.
    }
    else {
       m_pixelSpacePointsKey = (*itr).second; // Use the setting.
    }

    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------

  StatusCode PixelSpNtComponent::book(void) {

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
    
    m_tree->Branch("pixel_nsp", &m_nsp, "npixel_nsp/I");
    
    m_tree->Branch("pixel_sp_barrel_ec",     &m_sp_barrel_ec);
    m_tree->Branch("pixel_sp_layer_disk",    &m_sp_layer_disk);
    m_tree->Branch("pixel_sp_phi_module",    &m_sp_phi_module);
    m_tree->Branch("pixel_sp_eta_module",    &m_sp_eta_module);
    m_tree->Branch("pixel_sp_hashid_module", &m_sp_hashid_module);
    m_tree->Branch("pixel_sp_global_x",      &m_sp_global_x);
    m_tree->Branch("pixel_sp_global_y",      &m_sp_global_y);
    m_tree->Branch("pixel_sp_global_z",      &m_sp_global_z);
    
    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------
  
  StatusCode PixelSpNtComponent::fill(void) {
    
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

    const SpacePointContainer* pixelSpCont;
    sc = (*m_evtStore)->retrieve(  pixelSpCont, m_pixelSpacePointsKey );
    if(sc.isFailure() || !pixelSpCont) {
      if (!m_warningPrinted) {
	(*m_msg) << MSG::WARNING <<  m_pixelSpacePointsKey <<  " requested but not found." << endreq;
	m_warningPrinted = true;
      }
      return StatusCode::SUCCESS;
    }
    
    m_nsp = 0;
    // Loop over the container
    SpacePointContainer::const_iterator itr =  pixelSpCont->begin();
    SpacePointContainer::const_iterator itr_end =  pixelSpCont->end();     

    for( ; itr!=itr_end; ++itr ){


      Identifier id = (*itr)->identify();

      // Cache the values to flatten the data structure.
      short barrel_ec  = m_pixelHelper->barrel_ec(id);
      short layer_disk = m_pixelHelper->layer_disk(id);
      short phi_module = m_pixelHelper->phi_module(id);
      short eta_module = m_pixelHelper->eta_module(id);
      int   hash_id    = m_pixelHelper->wafer_hash(id);

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
