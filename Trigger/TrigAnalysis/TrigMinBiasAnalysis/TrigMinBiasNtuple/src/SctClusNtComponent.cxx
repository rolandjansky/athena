/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMinBiasNtuple/SctClusNtComponent.h"
#include "TrigNtupleBase/NtupleAlgorithm.h"

#include "InDetPrepRawData/SCT_ClusterContainer.h"  

#include "InDetIdentifier/SCT_ID.h"

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/MsgStream.h"

namespace NtComponent {
  
  //---------------------------------------------------------------------

  SctClusNtComponent::SctClusNtComponent(NtupleAlgorithm *mainAlg,
					 NtComponentParameters parameters):
    NtupleComponent(mainAlg, parameters) {
  }
  
  //---------------------------------------------------------------------

  SctClusNtComponent::~SctClusNtComponent() {
  }

  //---------------------------------------------------------------------

  StatusCode SctClusNtComponent::setupServices(void) {
    StatusCode sc;
    
    if((sc = NtupleComponent::setupServices()).isFailure()) return sc;
    
    // Get a Sct identifier helper
    sc = (*m_detStore)->retrieve(m_sctHelper, "SCT_ID");
    if(sc.isFailure()) {
      (*m_msg) << MSG::ERROR << "Can't get " << m_sctHelper << endreq;
      return StatusCode::FAILURE;
    }

    // Set the key from python or use the default.
    std::map<std::string,std::string>::iterator itr;
    itr = m_parameters.containerKeys.find("SCT_ClusterContainer"); // Search for StoreGate class.
    if(itr == m_parameters.containerKeys.end()) {
      m_sctSiClustersKey = "SCT_Clusters"; // The default when no key is found.
    }
    else {
      m_sctSiClustersKey = (*itr).second; // Use the setting.
    }

    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------

  StatusCode SctClusNtComponent::book(void) {

    // Create new objects but do not delete them such that they can be
    // stored in the TTree
    m_clus_barrel_ec     = new std::vector<short>();
    m_clus_layer_disk    = new std::vector<short>();
    m_clus_phi_module    = new std::vector<short>();
    m_clus_eta_module    = new std::vector<short>();
    m_clus_hashid_module = new std::vector<int>();
    m_clus_size          = new std::vector<int>();
    m_clus_global_x      = new std::vector<float>();
    m_clus_global_y      = new std::vector<float>();
    m_clus_global_z      = new std::vector<float>();
    
    m_tree->Branch("sct_nclus",              &m_nclus, "nsct_nclus/I");
    m_tree->Branch("sct_clus_barrel_ec",     &m_clus_barrel_ec);
    m_tree->Branch("sct_clus_layer_disk",    &m_clus_layer_disk);
    m_tree->Branch("sct_clus_phi_module",    &m_clus_phi_module);
    m_tree->Branch("sct_clus_eta_module",    &m_clus_eta_module);
    m_tree->Branch("sct_clus_hashid_module", &m_clus_hashid_module);
    m_tree->Branch("sct_clus_size",          &m_clus_size);
    m_tree->Branch("sct_clus_global_x",      &m_clus_global_x);
    m_tree->Branch("sct_clus_global_y",      &m_clus_global_y);
    m_tree->Branch("sct_clus_global_z",      &m_clus_global_z);
    
    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------
  
  StatusCode SctClusNtComponent::fill(void) {
  
    // Clear ntuple variables
    m_nclus = 0;
    m_clus_barrel_ec->clear();
    m_clus_layer_disk->clear();
    m_clus_phi_module->clear();
    m_clus_eta_module->clear();
    m_clus_hashid_module->clear();
    m_clus_size->clear();
    m_clus_global_x->clear();
    m_clus_global_y->clear();
    m_clus_global_z->clear();
    
    StatusCode sc; 

    const InDet::SCT_ClusterContainer *sctClusterContainer=0;
    sc = (*m_evtStore)->retrieve( sctClusterContainer, m_sctSiClustersKey );
    if( sc.isFailure() || !sctClusterContainer) {
      if (!m_warningPrinted) {
	(*m_msg) << MSG::WARNING << m_sctSiClustersKey << " requested but not found."  << endreq;
	m_warningPrinted = true;
      }
      return StatusCode::SUCCESS;
    }



    // no ToT or charge or ganged info for SCT

    // Loop over the space point container
    InDet::SCT_ClusterContainer::const_iterator itr = sctClusterContainer->begin();
    InDet::SCT_ClusterContainer::const_iterator itr_end = sctClusterContainer->end();     
    for(; itr!=itr_end; ++itr){
      // Loop over the collection
      InDet::SCT_ClusterCollection::const_iterator itr_clusCol = (*itr)->begin();
      InDet::SCT_ClusterCollection::const_iterator itr_clusCol_end = (*itr)->end();

      for(; itr_clusCol!=itr_clusCol_end; ++itr_clusCol) {

	Identifier id = (*itr_clusCol)->identify();

	m_clus_barrel_ec->push_back(m_sctHelper->barrel_ec(id));
	m_clus_layer_disk->push_back(m_sctHelper->layer_disk(id));
	m_clus_phi_module->push_back(m_sctHelper->phi_module(id));
	m_clus_eta_module->push_back(m_sctHelper->eta_module(id));
	m_clus_hashid_module->push_back((*itr_clusCol)->detectorElement()->identifyHash());
	m_clus_size->push_back((*itr_clusCol)->rdoList().size());
	m_clus_global_x->push_back((*itr_clusCol)->globalPosition().x());
	m_clus_global_y->push_back((*itr_clusCol)->globalPosition().y());
	m_clus_global_z->push_back((*itr_clusCol)->globalPosition().z());


	// Increment the number of clusters
	m_nclus++;

      }
    }

    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------
}
