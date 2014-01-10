/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMinBiasNtuple/PixelClusNtComponent.h"
#include "TrigNtupleBase/NtupleAlgorithm.h"

#include "InDetPrepRawData/PixelClusterContainer.h"         

#include "InDetIdentifier/PixelID.h"

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/MsgStream.h"

namespace NtComponent {
  
  //---------------------------------------------------------------------

  PixelClusNtComponent::PixelClusNtComponent(NtupleAlgorithm *mainAlg,
					     NtComponentParameters parameters):
    NtupleComponent(mainAlg, parameters) {
  }
  
  //---------------------------------------------------------------------

  PixelClusNtComponent::~PixelClusNtComponent() {
  }

  //---------------------------------------------------------------------

  StatusCode PixelClusNtComponent::setupServices(void) {
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
    itr = m_parameters.containerKeys.find("PixelClusterContainer"); // Search for StoreGate class.
    if(itr == m_parameters.containerKeys.end()) {
      m_pixelSiClustersKey = "PixelClusters"; // The default when no key is found.
    }
    else {
      m_pixelSiClustersKey = (*itr).second; // Use the setting.
    }

    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------

  StatusCode PixelClusNtComponent::book(void) {

    // Create new objects but do not delete them such that they can be
    // stored in the TTree
    m_clus_barrel_ec     = new std::vector<short>();
    m_clus_layer_disk    = new std::vector<short>();
    m_clus_phi_module    = new std::vector<short>();
    m_clus_eta_module    = new std::vector<short>();
    m_clus_hashid_module = new std::vector<int>();
    m_clus_size          = new std::vector<int>();
    m_clus_tot           = new std::vector<int>();
    m_clus_charge        = new std::vector<float>();
    m_clus_ganged        = new std::vector<bool>();
    m_clus_global_x      = new std::vector<float>();
    m_clus_global_y      = new std::vector<float>();
    m_clus_global_z      = new std::vector<float>();

    m_tree->Branch("pixel_nclus",              &m_nclus, "npixel_nclus/I");
    m_tree->Branch("pixel_clus_barrel_ec",     &m_clus_barrel_ec);
    m_tree->Branch("pixel_clus_layer_disk",    &m_clus_layer_disk);
    m_tree->Branch("pixel_clus_phi_module",    &m_clus_phi_module);
    m_tree->Branch("pixel_clus_eta_module",    &m_clus_eta_module);
    m_tree->Branch("pixel_clus_hashid_module", &m_clus_hashid_module);
    m_tree->Branch("pixel_clus_size",          &m_clus_size);
    m_tree->Branch("pixel_clus_tot",           &m_clus_tot);
    m_tree->Branch("pixel_clus_charge",        &m_clus_charge);
    m_tree->Branch("pixel_clus_ganged",        &m_clus_ganged);
    m_tree->Branch("pixel_clus_global_x",      &m_clus_global_x);
    m_tree->Branch("pixel_clus_global_y",      &m_clus_global_y);
    m_tree->Branch("pixel_clus_global_z",      &m_clus_global_z);
    
    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------
  
  StatusCode PixelClusNtComponent::fill(void) {
    
    // Clear ntuple variables 
    m_nclus = -9;
    m_clus_barrel_ec->clear();
    m_clus_layer_disk->clear();
    m_clus_phi_module->clear();
    m_clus_eta_module->clear();
    m_clus_hashid_module->clear();
    m_clus_size->clear();
    m_clus_tot->clear();
    m_clus_charge->clear();
    m_clus_ganged->clear();
    m_clus_global_x->clear();
    m_clus_global_y->clear();
    m_clus_global_z->clear();

    StatusCode sc;

    const InDet::PixelClusterContainer *pixelClusterContainer; 
    sc = (*m_evtStore)->retrieve( pixelClusterContainer, m_pixelSiClustersKey );
    if(sc.isFailure() || !pixelClusterContainer) {
      if (!m_warningPrinted) {
	(*m_msg) << MSG::WARNING << m_pixelSiClustersKey << " requested but not found."  << endreq;
	m_warningPrinted = true;
      }
      return StatusCode::SUCCESS;
    }
    
    m_nclus = 0;
    // Loop over the container
    InDet::PixelClusterContainer::const_iterator itr = pixelClusterContainer->begin();
    InDet::PixelClusterContainer::const_iterator itr_end = pixelClusterContainer->end();
    for( ; itr!=itr_end; ++itr ){

      // Loop over the collection
      InDet::PixelClusterCollection::const_iterator itr_col = (*itr)->begin();
      InDet::PixelClusterCollection::const_iterator itr_col_end = (*itr)->end();
      for( ; itr_col!=itr_col_end; ++itr_col ){
	Identifier id = (*itr_col)->identify();
	m_clus_barrel_ec->push_back(m_pixelHelper->barrel_ec(id));
	m_clus_layer_disk->push_back(m_pixelHelper->layer_disk(id));
	m_clus_phi_module->push_back(m_pixelHelper->phi_module(id));
	m_clus_eta_module->push_back(m_pixelHelper->eta_module(id));
	m_clus_hashid_module->push_back((*itr_col)->detectorElement()->identifyHash());
	m_clus_size->push_back((*itr_col)->rdoList().size()); // Cluster size, note this is 0 in MC ESDs
	m_clus_tot->push_back((*itr_col)->totalToT()); // Total time over threshold
	m_clus_charge->push_back((*itr_col)->totalCharge());
	m_clus_ganged->push_back((*itr_col)->gangedPixel());
	m_clus_global_x->push_back((*itr_col)->globalPosition().x());
	m_clus_global_y->push_back((*itr_col)->globalPosition().y());
	m_clus_global_z->push_back((*itr_col)->globalPosition().z());

	// Increment the number of clusters
	m_nclus++;
      }
    }

    return StatusCode::SUCCESS;
  }
  
  //---------------------------------------------------------------------
}
