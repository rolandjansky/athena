/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMinBiasNtuple/TrackletNtComponent.h"
#include "TrigNtupleBase/NtupleAlgorithm.h"

#include "Particle/TrackParticleContainer.h"

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/MsgStream.h"
 
namespace NtComponent {
  
  //---------------------------------------------------------------------

  TrackletNtComponent::TrackletNtComponent(NtupleAlgorithm *mainAlg,
				     NtComponentParameters parameters):
    NtupleComponent(mainAlg, parameters) {
  }
  
  //---------------------------------------------------------------------

  TrackletNtComponent::~TrackletNtComponent() {
  }

  //---------------------------------------------------------------------

  StatusCode TrackletNtComponent::book(void) {

    // Set the key from python or use the default.
    std::map<std::string,std::string>::iterator itr;
    itr = m_parameters.containerKeys.find("PixelTrackletTrackParticles"); // Search for StoreGate class.
    if(itr == m_parameters.containerKeys.end()) {
      m_trackParticleContainerKey = "PixelTrackletTrackParticles"; // The default when no key is found.
    }
    else {
      m_trackParticleContainerKey = (*itr).second; // Use the setting.
    }
    
    // Create new objects but do not delete them such that they can be
    // stored in the TTree
    m_trkl_eta = new std::vector<float>();
    m_trkl_phi = new std::vector<float>();
    m_trkl_deta = new std::vector<float>();
    m_trkl_dphi = new std::vector<float>();

    // Basic track parameters
    m_tree->Branch("n_trkl",&m_ntrkl,"n_trkl/I");
    m_tree->Branch("trkl_eta", &m_trkl_eta);
    m_tree->Branch("trkl_phi", &m_trkl_phi);
    m_tree->Branch("trkl_deta", &m_trkl_deta);
    m_tree->Branch("trkl_dphi", &m_trkl_dphi);

    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------
  
  StatusCode TrackletNtComponent::fill(void) {
    
    // Clear the ntuple variables
    m_trkl_eta->clear();
    m_trkl_phi->clear();
    m_trkl_deta->clear();
    m_trkl_dphi->clear();
    m_ntrkl = 0;
    // retrieve reconstructed tracks
    const Rec::TrackParticleContainer *trackParticleContainer = 0;

    StatusCode sc = (*m_evtStore)->retrieve(trackParticleContainer, m_trackParticleContainerKey);
    if (sc.isFailure() || !trackParticleContainer) {
      if (!m_warningPrinted) {	
	(*m_msg) << MSG::WARNING << m_trackParticleContainerKey << " requested but not found." << endreq;
	m_warningPrinted = true;
      }
      return StatusCode::SUCCESS;
    } 
    else 
      {
	(*m_msg) << MSG::DEBUG << "TracksCollection " << m_trackParticleContainerKey << " found." <<endreq;
      }
    
    Rec::TrackParticleContainer::const_iterator tpb = trackParticleContainer->begin();
    Rec::TrackParticleContainer::const_iterator tpe = trackParticleContainer->end();
    float eta,phi,deta,dphi;
    for (;tpb!=tpe;++tpb)
      {
	eta = (*tpb)->eta();
	phi = (*tpb)->phi();
	deta = 0;
	// 0.0111 might change.  
	dphi = 0.0111/(*tpb)->pt();
	m_trkl_eta->push_back(eta);
	m_trkl_phi->push_back(phi);
	m_trkl_deta->push_back(deta);
	m_trkl_dphi->push_back(dphi);
	m_ntrkl++;
      }
     (*m_msg) << MSG::INFO << "Added " << m_ntrkl << " pixel tracklets!" << endreq;
      
    
    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------
}
