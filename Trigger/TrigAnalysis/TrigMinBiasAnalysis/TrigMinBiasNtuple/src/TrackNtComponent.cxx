/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMinBiasNtuple/TrackNtComponent.h"
#include "TrigNtupleBase/NtupleAlgorithm.h"

#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/MsgStream.h"
 
namespace NtComponent {
  
  //---------------------------------------------------------------------

  TrackNtComponent::TrackNtComponent(NtupleAlgorithm *mainAlg,
				     NtComponentParameters parameters):
    NtupleComponent(mainAlg, parameters) {
  }
  
  //---------------------------------------------------------------------

  TrackNtComponent::~TrackNtComponent() {
  }

  //---------------------------------------------------------------------

  StatusCode TrackNtComponent::book(void) {

    // Set the key from python or use the default.
    std::map<std::string,std::string>::iterator itr;
    itr = m_parameters.containerKeys.find("TrackCollection"); // Search for StoreGate class.
    if(itr == m_parameters.containerKeys.end()) {
      m_trackCollectionKey = "Tracks"; // The default when no key is found.
    }
    else {
      m_trackCollectionKey = (*itr).second; // Use the setting.
    }
    
    // Create new objects but do not delete them such that they can be
    // stored in the TTree
    m_trk_d0 = new std::vector<float>();
    m_trk_z0 = new std::vector<float>();
    m_trk_phi0 = new std::vector<float>();
    m_trk_theta = new std::vector<float>();
    m_trk_qOverP = new std::vector<float>();
    m_trk_eta = new std::vector<float>();
    m_trk_d0_err = new std::vector<float>();
    m_trk_z0_err = new std::vector<float>();
    m_trk_phi0_err = new std::vector<float>();
    m_trk_theta0_err = new std::vector<float>();
    m_trk_qOverP_err = new std::vector<float>();

    // Basic track parameters
    m_tree->Branch("trk_d0", &m_trk_d0);
    m_tree->Branch("trk_z0", &m_trk_z0);
    m_tree->Branch("trk_phi0", &m_trk_phi0);
    m_tree->Branch("trk_theta", &m_trk_theta);
    m_tree->Branch("trk_qOverP", &m_trk_qOverP);
    m_tree->Branch("trk_eta", &m_trk_eta);
    m_tree->Branch("trk_d0_err", &m_trk_d0_err);
    m_tree->Branch("trk_z0_err", &m_trk_z0_err);
    m_tree->Branch("trk_phi0_err", &m_trk_phi0_err);
    m_tree->Branch("trk_theta0_err", &m_trk_theta0_err);
    m_tree->Branch("trk_qOverP_err", &m_trk_qOverP_err);

    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------
  
  StatusCode TrackNtComponent::fill(void) {
    
    // Clear the ntuple variables
    m_trk_d0->clear();
    m_trk_z0->clear();
    m_trk_phi0->clear();
    m_trk_theta->clear();
    m_trk_qOverP->clear();
    m_trk_eta->clear();
    m_trk_d0_err->clear();
    m_trk_z0_err->clear();
    m_trk_phi0_err->clear();
    m_trk_theta0_err->clear();
    m_trk_qOverP_err->clear();
    
    // retrieve reconstructed tracks
    const TrackCollection *trackCollection = 0;

    StatusCode sc = (*m_evtStore)->retrieve(trackCollection, m_trackCollectionKey);
    if (sc.isFailure() || !trackCollection) {
      if (!m_warningPrinted) {
      (*m_msg) << MSG::WARNING << m_trackCollectionKey << " requested but not found." << endreq;
      m_warningPrinted = true;
      }
      return StatusCode::SUCCESS;
    } 
    else {
      (*m_msg) << MSG::DEBUG << "TracksCollection " << m_trackCollectionKey << " found." <<endreq;
    }
    
    std::map<Trk::Track *, long > trackIndexMap;
    long itrk = 0;

    // Loop over the track collection.
    TrackCollection::const_iterator itr = trackCollection->begin();
    TrackCollection::const_iterator itr_end = trackCollection->end();
    const Trk::Perigee *perigee = 0;
    
    for(;itr!=itr_end;++itr) {
      trackIndexMap.insert(std::make_pair((*itr),itrk));
      itrk++;

      perigee = (*itr)->perigeeParameters();
      if(!perigee) {
	(*m_msg) << MSG::DEBUG << "No perigee parameters found." << endreq;
	
	// Fill the vectors with -999 to flag this error.
	m_trk_d0->push_back(-999.);
	m_trk_z0->push_back(-999.);
	m_trk_phi0->push_back(-999.);
	m_trk_theta->push_back(-999.);
	m_trk_qOverP->push_back(-999.);
	m_trk_eta->push_back(-999.);
	m_trk_d0_err->push_back(-999);
	m_trk_z0_err->push_back(-999);
	m_trk_phi0_err->push_back(-999);
	m_trk_theta0_err->push_back(-999);
	m_trk_qOverP_err->push_back(-999);
	continue;
      }
      
      // Fill the perigee information
      m_trk_d0->push_back(perigee->parameters()[Trk::d0]);
      m_trk_z0->push_back(perigee->parameters()[Trk::z0]);
      m_trk_phi0->push_back(perigee->parameters()[Trk::phi0]);
      m_trk_theta->push_back(perigee->parameters()[Trk::theta]);
      m_trk_qOverP->push_back(perigee->parameters()[Trk::qOverP]);
      m_trk_eta->push_back(perigee->eta());
    
      // Get the measured track parameters and retrieve the errors
      const AmgSymMatrix(5)* cov = perigee->covariance();
      if(!cov) {
	(*m_msg) << MSG::DEBUG << "No covariance." << endreq;

	// Fill the vectors with -999 to flag this error.
	m_trk_d0_err->push_back(-999);
	m_trk_z0_err->push_back(-999);
	m_trk_phi0_err->push_back(-999);
	m_trk_theta0_err->push_back(-999);
	m_trk_qOverP_err->push_back(-999);
	continue;
      }

      // error of track parameters
      m_trk_d0_err->push_back(Amg::error(*cov,Trk::d0));
      m_trk_z0_err->push_back(Amg::error(*cov,Trk::z0));
      m_trk_phi0_err->push_back(Amg::error(*cov,Trk::phi0));
      m_trk_theta0_err->push_back(Amg::error(*cov,Trk::theta));
      m_trk_qOverP_err->push_back(Amg::error(*cov,Trk::qOverP));
    }
    
    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------
}
