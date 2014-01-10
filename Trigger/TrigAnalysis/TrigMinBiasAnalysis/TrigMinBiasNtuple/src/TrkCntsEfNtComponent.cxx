/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMinBiasNtuple/TrkCntsEfNtComponent.h"
#include "TrigNtupleBase/NtupleAlgorithm.h"

#include "TrigInDetEvent/TrigTrackCountsCollection.h"

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/MsgStream.h"

namespace NtComponent {
  
  //---------------------------------------------------------------------

  TrkCntsEfNtComponent::TrkCntsEfNtComponent(NtupleAlgorithm *mainAlg,
					     NtComponentParameters parameters):
    NtupleComponent(mainAlg, parameters) {
  }
  
  //---------------------------------------------------------------------

  TrkCntsEfNtComponent::~TrkCntsEfNtComponent() {
  }

  //---------------------------------------------------------------------

  StatusCode TrkCntsEfNtComponent::book(void) {

    // Set the key from python or use the default.
    std::map<std::string,std::string>::iterator itr;
    itr = m_parameters.containerKeys.find("TrigTrackCountsCollection"); // Search for StoreGate class.
    if(itr == m_parameters.containerKeys.end()) {
      m_trackCountsCollectionKey = "HLT_trackcounts"; // The default when no key is found.
    }
    else {
      m_trackCountsCollectionKey = (*itr).second; // Use the setting.
    }

    // Create new objects but do not delete them such that they can be
    // stored in the TTree
    m_z0_pt_contents = new std::vector<float>();
    m_eta_phi_contents = new std::vector<float>();

    m_tree->Branch("ef_trk_nElements", &m_nElements, "ef_trk_nElements/i");
    m_tree->Branch("ef_trk_cnts_hZ0Bins", &m_hZ0Bins, "ef_trk_cnts_hZ0Bins/i");
    m_tree->Branch("ef_trk_cnts_hZ0Min", &m_hZ0Min, "ef_trk_cnts_hZ0Min/F");
    m_tree->Branch("ef_trk_cnts_hZ0Max", &m_hZ0Max, "ef_trk_cnts_hZ0Max/F");
    m_tree->Branch("ef_trk_cnts_hPtBins",&m_hPtBins, "ef_trk_cnts_hPtBins/i");
    m_tree->Branch("ef_trk_cnts_hPtMin",&m_hPtMin, "ef_trk_cnts_hPtMin/F");
    m_tree->Branch("ef_trk_cnts_hPtMax",&m_hPtMax, "ef_trk_cnts_hPtMax/F");
    m_tree->Branch("ef_trk_cnts_hEtaBins",&m_hEtaBins, "ef_trk_cnts_hEtaBins/i");
    m_tree->Branch("ef_trk_cnts_hEtaMin",&m_hEtaMin, "ef_trk_cnts_hEtaMin/F");
    m_tree->Branch("ef_trk_cnts_hEtaMax",&m_hEtaMax, "ef_trk_cnts_hEtaMax/F");
    m_tree->Branch("ef_trk_cnts_hPhiBins",&m_hPhiBins, "ef_trk_cnts_hPhiBins/i");
    m_tree->Branch("ef_trk_cnts_hPhiMin",&m_hPhiMin, "ef_trk_cnts_hPhiMin/F");
    m_tree->Branch("ef_trk_cnts_hPhiMax",&m_hPhiMax, "ef_trk_cnts_hPhiMax/F");
    m_tree->Branch("ef_trk_cnts_z0_pt_contents", &m_z0_pt_contents);
    m_tree->Branch("ef_trk_cnts_eta_phi_contents", &m_eta_phi_contents);

    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------
  
  StatusCode TrkCntsEfNtComponent::fill(void) {
    StatusCode sc;
    const TrigTrackCountsCollection *trkCountsCollection = 0;

    // Clear the ntuple variables
    m_nElements = 0;
    m_hZ0Bins = 0;
    m_hZ0Min = 0;
    m_hZ0Max = 0;
    m_hPtBins = 0;
    m_hPtMin = 0;
    m_hPtMax = 0;
    m_hEtaBins = 0;
    m_hEtaMin = 0;
    m_hEtaMax = 0;
    m_hPhiBins = 0;
    m_hPhiMin = 0;
    m_hPhiMax = 0;
    m_z0_pt_contents->clear();
    m_eta_phi_contents->clear();

    // Container is present only if the seed trigger item has fired.
    sc=(*m_evtStore)->retrieve(trkCountsCollection, m_trackCountsCollectionKey);
    if( sc.isFailure()  || !trkCountsCollection ) {
      if (!m_warningPrinted) {
	(*m_msg) << MSG::WARNING <<  m_trackCountsCollectionKey <<  " requested but not found." << endreq;
	m_warningPrinted = true;
      }
      return StatusCode::SUCCESS;
    }

    m_nElements = trkCountsCollection->size();
    (*m_msg) << MSG::DEBUG << "There are " << m_nElements << " TrigTrackCounts entries." << endreq;

    TrigTrackCountsCollection::const_iterator itr = trkCountsCollection->begin();
    TrigTrackCountsCollection::const_iterator itr_end = trkCountsCollection->end();
    TrigHisto2D z0_pt;
    TrigHisto2D eta_phi;

    // The TE is full scan and not connected to an RoI.  All TEs
    // within a given event are therefore the same.  Just get the first TE.
    if(itr != itr_end) {

      (*m_msg) << MSG::DEBUG << "Dumping the first TrigTrackCountsCollection entry." << endreq;
  
      z0_pt = (*itr)->z0_pt();
      m_hZ0Bins = z0_pt.nbins_x();
      m_hZ0Min = z0_pt.min_x();
      m_hZ0Max = z0_pt.max_x();
      m_hPtBins = z0_pt.nbins_y();
      m_hPtMin = z0_pt.min_y();
      m_hPtMax = z0_pt.max_y();
      *m_z0_pt_contents = z0_pt.contents();

      eta_phi = (*itr)->eta_phi();
      m_hEtaBins = eta_phi.nbins_x();
      m_hEtaMin = eta_phi.min_x();
      m_hEtaMax = eta_phi.max_x();
      m_hPhiBins = eta_phi.nbins_y();
      m_hPhiMin = eta_phi.min_y();
      m_hPhiMax = eta_phi.max_y();
      *m_eta_phi_contents = eta_phi.contents();
    }
    
    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------
}
