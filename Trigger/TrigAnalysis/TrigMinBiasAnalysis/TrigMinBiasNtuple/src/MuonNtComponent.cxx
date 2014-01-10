/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMinBiasNtuple/MuonNtComponent.h"
#include "TrigNtupleBase/NtupleAlgorithm.h"

#include "TileEvent/TileCosmicMuon.h"
#include "TileEvent/TileCosmicMuonContainer.h"

#include "Particle/TrackParticle.h"
#include "Particle/TrackParticleContainer.h"

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/MsgStream.h"

namespace NtComponent {

  //---------------------------------------------------------------------
  
  MuonNtComponent::MuonNtComponent(NtupleAlgorithm *mainAlg,
				   NtComponentParameters parameters):
    NtupleComponent(mainAlg, parameters) {
  }
  
  //---------------------------------------------------------------------
  
  MuonNtComponent::~MuonNtComponent() {
  }
  
  //---------------------------------------------------------------------
  
  StatusCode MuonNtComponent::setupServices(void) {
    StatusCode sc;

    /*
    // Get a copy of the BCM_RDO_Container key.
    sc = m_mainAlg->containerKey("Muon_TrackParticleContainer", &m_MuSpecMuonContName);
    if(sc.isFailure()) {
      (*m_msg) << MSG::ERROR << "Container key for BCM_RDO_Container is not defined." << endreq;
      return StatusCode::FAILURE;
    }
    */

    //m_CaloMuonContName   = m_mainAlg->caloMuonContainerKey();
    //m_MuSpecMuonContName = m_mainAlg->muspecMuonContainerKey();
    //useCaloMuon           = m_mainAlg->useCaloMuon();
    //useMuSpecMuon         = m_mainAlg->useMuSpecMuon();

    (*m_msg) << MSG::ERROR << "This ntuple component needs to be broken up." << endreq;
    return StatusCode::FAILURE;

    // Initialise base class services
    if((sc = NtupleComponent::setupServices()).isFailure()) return sc;
    
    return StatusCode::SUCCESS;
  }
  
  //---------------------------------------------------------------------
  
  StatusCode MuonNtComponent::book(void) {
    StatusCode sc;
    
    // Create new objects but do not delete them such that they can be
    // stored in the TTree
    
    // TileMuonFitter (CaloMuons)
    if(useCaloMuon) {
      m_calo_time = new std::vector<double>();
      m_calo_theta = new std::vector<double>();
      m_calo_phi = new std::vector<double>();
      m_calo_x0 = new std::vector<double>();
      m_calo_y0 = new std::vector<double>();
      m_calo_z0 = new std::vector<double>();
      m_calo_quality = new std::vector<double>();
      m_tree->Branch("calomuon_ncell",   &m_calo_ncell,    "calomuon_ncell/I");
      m_tree->Branch("calomuon_time",    &m_calo_time);
      m_tree->Branch("calomuon_theta",   &m_calo_theta);
      m_tree->Branch("calomuon_phi",     &m_calo_phi);
      m_tree->Branch("calomuon_x0",      &m_calo_x0);
      m_tree->Branch("calomuon_y0",      &m_calo_y0);
      m_tree->Branch("calomuon_z0",      &m_calo_z0);
      m_tree->Branch("calomuon_quality", &m_calo_y0);
    }
    
    // MuonSpectrometer Combined Muons
    if(useMuSpecMuon) {
      m_muspec_a0 = new std::vector<double>();
      m_muspec_x0 = new std::vector<double>();
      m_muspec_y0 = new std::vector<double>();
      m_muspec_z0 = new std::vector<double>();
      m_muspec_phi = new std::vector<double>();
      m_muspec_theta = new std::vector<double>();
      m_muspec_chi2 = new std::vector<double>();
      m_muspec_dof = new std::vector<double>();
      m_tree->Branch("muspec_a0",    &m_muspec_a0);
      m_tree->Branch("muspec_x0",    &m_muspec_x0);
      m_tree->Branch("muspec_y0",    &m_muspec_y0);
      m_tree->Branch("muspec_z0",    &m_muspec_z0);
      m_tree->Branch("muspec_phi",   &m_muspec_phi);
      m_tree->Branch("muspec_theta", &m_muspec_theta);
      m_tree->Branch("muspec_chi2",  &m_muspec_chi2);
      m_tree->Branch("muspec_dof",   &m_muspec_dof);
    }
    
    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------
  
  StatusCode MuonNtComponent::fill(void) {
    StatusCode sc;
    
    // TileMuonFitter (CaloMuons)
    if(useCaloMuon) {
      const TileCosmicMuonContainer *caloMuonCnt = 0;
      sc=(*m_evtStore)->retrieve(caloMuonCnt, m_CaloMuonContName);
      if( sc.isFailure()  || !caloMuonCnt ) {
	(*m_msg) << MSG::ERROR << "Error retrieving " << m_CaloMuonContName << endreq;
	return StatusCode::FAILURE;
      }
      
      m_calo_ncell=0;
      m_calo_time->clear();
      m_calo_theta->clear();
      m_calo_phi->clear();
      m_calo_x0->clear();
      m_calo_y0->clear();
      m_calo_z0->clear();
      m_calo_quality->clear();
      
      TileCosmicMuonContainer::const_iterator itr = caloMuonCnt->begin();
      TileCosmicMuonContainer::const_iterator itr_end = caloMuonCnt->end();
      for(; itr != itr_end; ++itr) {
	m_calo_time->push_back((*itr)->GetTime());
	m_calo_theta->push_back((*itr)->GetDirectionTheta());
	m_calo_phi->push_back((*itr)->GetDirectionPhi());
	m_calo_x0->push_back((*itr)->GetPositionX());
	m_calo_y0->push_back((*itr)->GetPositionY());
	m_calo_z0->push_back((*itr)->GetPositionZ());
	m_calo_quality->push_back((*itr)->GetFitQuality());
	
	m_calo_ncell++;
      }
    } // end CaloMuon
    
    // MuonSpectrometer Combined Muons
    if(useMuSpecMuon)
      {
	const Rec::TrackParticleContainer *muspecMuonCnt = 0;
	sc=(*m_evtStore)->retrieve(muspecMuonCnt, m_MuSpecMuonContName);
	if( sc.isFailure()  || !muspecMuonCnt ) {
	  (*m_msg) << MSG::ERROR << "Error retrieving " << m_MuSpecMuonContName << endreq;
	  return StatusCode::FAILURE;
	}
	
	m_muspec_a0->clear();
	m_muspec_x0->clear();
	m_muspec_y0->clear();
	m_muspec_z0->clear();
	m_muspec_theta->clear();
	m_muspec_phi->clear();
	m_muspec_chi2->clear();
	m_muspec_dof->clear();
	
	Rec::TrackParticleContainer::const_iterator itr = muspecMuonCnt->begin();
	Rec::TrackParticleContainer::const_iterator itr_end = muspecMuonCnt->end();
	for(; itr != itr_end; ++itr) {
	  m_muspec_a0->push_back((*itr)->perigee()->parameters()[0]);
	  m_muspec_x0->push_back((*itr)->perigee()->position().x());
	  m_muspec_y0->push_back((*itr)->perigee()->position().y());
	  m_muspec_z0->push_back((*itr)->perigee()->position().z());
	  m_muspec_phi->push_back((*itr)->perigee()->parameters()[2]);
	  m_muspec_theta->push_back((*itr)->perigee()->parameters()[3]);
	  m_muspec_chi2->push_back((*itr)->fitQuality()->chiSquared());
	  m_muspec_dof->push_back((*itr)->fitQuality()->numberDoF());
	}
      } // end MuonSpecMuon
    
    return StatusCode::SUCCESS;
  }
  
  //---------------------------------------------------------------------
}
