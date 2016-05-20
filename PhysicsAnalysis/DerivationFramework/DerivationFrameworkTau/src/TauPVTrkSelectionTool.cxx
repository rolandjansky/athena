/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TauPVTrkSelectionTool.cxx
// Author: Evelina Bouhova-Thacker (e.bouhova@cern.ch)
//
///////////////////////////////////////////////////////////////////

#include "DerivationFrameworkTau/TauPVTrkSelectionTool.h"
#include "xAODTracking/Vertex.h"
#include "TauAnalysisTools/ITauTruthTrackMatchingTool.h"
#include "xAODTau/TauJetContainer.h"

namespace DerivationFramework {

  TauPVTrkSelectionTool::TauPVTrkSelectionTool(const std::string& t, const std::string& n, const IInterface* p) : 
    AthAlgTool(t,n,p),
    m_tauPVTracksContainerName("TauPVTracks"),
    m_tauContainerName("TauJets"),
    m_useTruth(false),
    m_maxDeltaR(0.2),
    m_minPt(20000.),
    m_T3MT("TauAnalysisTools::TauTruthTrackMatchingTool")
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    declareProperty("TauPVTracksContainerName", m_tauPVTracksContainerName);
    declareProperty("TauContainerName", m_tauContainerName);
    declareProperty("UseTrueTracks", m_useTruth);
    declareProperty("maxDeltaR", m_maxDeltaR);
    declareProperty("minPt", m_minPt);
    declareProperty("TauTruthTrackMatchingTool", m_T3MT);
  }

  StatusCode TauPVTrkSelectionTool::initialize()
  {
    if (m_tauPVTracksContainerName=="") {
      ATH_MSG_ERROR("No SG name provided for the output of the tau PV track selector tool!");
      return StatusCode::FAILURE;
    }
    CHECK( m_T3MT.retrieve() );
    return StatusCode::SUCCESS;
  }

  StatusCode TauPVTrkSelectionTool::finalize()
  {
    return StatusCode::SUCCESS;
  }

  StatusCode TauPVTrkSelectionTool::addBranches() const
  {
    // check that container we want to write in SG does not yet exist
    if (evtStore()->contains<std::vector<float> >(m_tauPVTracksContainerName)) {
      ATH_MSG_ERROR("Tool is attempting to write a StoreGate key " << m_tauPVTracksContainerName << " which already exists. Please use a different key");
      return StatusCode::FAILURE;
    }

    xAOD::TrackParticleContainer*       tauPVTracks(0);

    CHECK(select(tauPVTracks));
    ATH_MSG_DEBUG ("number of tau tracks for PV refit " << tauPVTracks->size());

    if (!evtStore()->contains<xAOD::TrackParticleContainer>(m_tauPVTracksContainerName))
      CHECK(evtStore()->record(tauPVTracks, m_tauPVTracksContainerName));

    return StatusCode::SUCCESS;
  }  

  StatusCode TauPVTrkSelectionTool::select(xAOD::TrackParticleContainer*& tauPVTracks) const
  {
    tauPVTracks = new xAOD::TrackParticleContainer(SG::VIEW_ELEMENTS);

    // Get tau collection from StoreGate
    const xAOD::TauJetContainer* tau_cont = evtStore()->retrieve< const xAOD::TauJetContainer >( m_tauContainerName );
    if (tau_cont == 0) return StatusCode::SUCCESS;
    ATH_MSG_DEBUG ("number of taus " << tau_cont->size());
    // loop over tau container to select tracks for the PV refit
    for (auto pTau: *tau_cont ) {
      int tauNtracks = pTau->nTracks();
      bool good_tau = pTau->isTau(xAOD::TauJetParameters::JetBDTSigMedium);
      float tau_pt  = pTau->pt();
      float tau_eta = pTau->eta();
      bool pass_selection = false;
      if (tau_pt > m_minPt && (fabs(tau_eta) < 1.37 || (fabs(tau_eta) > 1.52 && fabs(tau_eta) < 2.5))) pass_selection = true;
      for (int i = 0; i < tauNtracks; i++) {
#ifdef XAODTAU_VERSIONS_TAUJET_V3_H
        const xAOD::TauTrack* tauTrk = pTau->track(i);
	const xAOD::TrackParticle* tauTrk_trk = tauTrk->track();
#else
        const xAOD::TrackParticle* tauTrk = pTau->track(i);
	const xAOD::TrackParticle* tauTrk_trk = tauTrk;
#endif	
        if (m_useTruth) {
          // identify tracks matched to tau decay products (hadrons only)
          if (!m_T3MT->classifyTrack(*tauTrk)) continue;
          if (tauTrk->auxdecor<char>("IsHadronicTrack")) tauPVTracks->push_back(const_cast<xAOD::TrackParticle*>(tauTrk_trk));
        } else {
          // use all tau tracks
          //TauTracks.push_back(const_cast<xAOD::TrackParticle*>(tauTrk));
          float dR = pTau->p4().DeltaR(tauTrk->p4());
          if (good_tau && pass_selection && dR < m_maxDeltaR) tauPVTracks->push_back(const_cast<xAOD::TrackParticle*>(tauTrk_trk));
        }
      }
    }
    ATH_MSG_DEBUG ("tauPVTracks size " << tauPVTracks->size());

    return StatusCode::SUCCESS; 
  }
}
