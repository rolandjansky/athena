/*
 Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "eflowRec/PFTauFlowElementAssoc.h"
#include "xAODTau/TauJetContainer.h"
#include "xAODTau/TauTrack.h"
#include "xAODPFlow/FlowElementContainer.h"
#include "xAODPFlow/FlowElement.h"

typedef ElementLink<xAOD::TauJetContainer> TauJetLink_t;
typedef ElementLink<xAOD::FlowElementContainer> FELink_t;

PFTauFlowElementAssoc::PFTauFlowElementAssoc(const std::string& name,
              ISvcLocator* pSvcLocator): AthReentrantAlgorithm(name, pSvcLocator)
{}

PFTauFlowElementAssoc::~PFTauFlowElementAssoc() {}

StatusCode PFTauFlowElementAssoc::initialize() {

  ATH_MSG_DEBUG("Initializing" << name() << "...");

  ATH_CHECK(m_tauNeutralFEWriteDecorKey.initialize());
  ATH_CHECK(m_tauChargedFEWriteDecorKey.initialize());
  ATH_CHECK(m_neutralFETauWriteDecorKey.initialize());
  ATH_CHECK(m_chargedFETauWriteDecorKey.initialize());

  ATH_CHECK(m_tauJetReadHandleKey.initialize());
  ATH_CHECK(m_neutralFEReadHandleKey.initialize());
  ATH_CHECK(m_chargedFEReadHandleKey.initialize());

  ATH_MSG_DEBUG("Initialization completed successfully");

  return StatusCode::SUCCESS;
}

/**
  This algorithm:
  1) Accesses the relevant FlowElement and TauJet containers
  2) Loops over the neutral FEs and matches them to the corresponding TauJet clusters, creating the ElementLinks
     and adding them to the JetETMissNeutralFlowElements container as a decoration
  3) Loops over the charged FEs and matches them to the corresponding TauJet tracks, creating the ElementLinks
     and adding them to the JetETMissChargedFlowElements container as a decoration
  4) Adds the decoration to the TauJet container containing a vector of ElementLinks to corresponding FEs
**/

StatusCode PFTauFlowElementAssoc::execute(const EventContext &ctx) const {

  // Write decoration handles for linking the TauJet container with the FlowElement container and vice versa
  SG::WriteDecorHandle< xAOD::TauJetContainer, std::vector<FELink_t> > tauNeutralFEWriteDecorHandle (m_tauNeutralFEWriteDecorKey,ctx);
  SG::WriteDecorHandle< xAOD::FlowElementContainer, std::vector<TauJetLink_t> > neutralFETauWriteDecorHandle (m_neutralFETauWriteDecorKey,ctx);
  SG::WriteDecorHandle< xAOD::TauJetContainer, std::vector<FELink_t> > tauChargedFEWriteDecorHandle (m_tauChargedFEWriteDecorKey,ctx);
  SG::WriteDecorHandle< xAOD::FlowElementContainer, std::vector<TauJetLink_t> > chargedFETauWriteDecorHandle (m_chargedFETauWriteDecorKey,ctx);
  // Read handles for the TauJet container and the FlowElement container
  SG::ReadHandle<xAOD::TauJetContainer> tauJetReadHandle (m_tauJetReadHandleKey,ctx);
  SG::ReadHandle<xAOD::FlowElementContainer> neutralFEReadHandle (m_neutralFEReadHandleKey,ctx);
  SG::ReadHandle<xAOD::FlowElementContainer> chargedFEReadHandle (m_chargedFEReadHandleKey,ctx);

  // Initialize flow element link containers
  std::vector<std::vector<FELink_t>> tauNeutralFEVec(tauJetReadHandle->size());
  std::vector<std::vector<FELink_t>> tauChargedFEVec(tauJetReadHandle->size());

  ////////////////////////////////////////////
  // Loop over the neutral flow elements
  ////////////////////////////////////////////
  for (const xAOD::FlowElement* FE : *neutralFETauWriteDecorHandle) {
    // Get the index of the flow element cluster
    size_t FEClusterIndex = FE->otherObjects().at(0)->index();

    std::vector<TauJetLink_t> FETauJetLinks;

    // Loop over the taus
    for (const xAOD::TauJet* tau : *tauNeutralFEWriteDecorHandle) {
      // Get the clusters associated to the tau
      std::vector< ElementLink<xAOD::IParticleContainer> > tauClusters = tau->clusterLinks();
      for (auto clusLink : tauClusters) {
        const xAOD::IParticle* clus = *clusLink;
        // Get the index of the cluster associated to the tau
        size_t tauClusterIndex = clus->index();

        // Link the tau and the neutral FE if the cluster indices match
        if (tauClusterIndex == FEClusterIndex) {
          FETauJetLinks.push_back( TauJetLink_t(*tauJetReadHandle,tau->index()) );
          tauNeutralFEVec.at(tau->index()).push_back( FELink_t(*neutralFEReadHandle, FE->index()) );
        }

      } // end tau cluster loop
    } // end tau loop

    // Add vector of elements links to the tau jets as a decoration to the FE container
    neutralFETauWriteDecorHandle (*FE) = FETauJetLinks;

  } // end neutral FE loop

  ////////////////////////////////////////////
  // Loop over the charged flow elements
  ////////////////////////////////////////////
  for (const xAOD::FlowElement* FE : *chargedFETauWriteDecorHandle) {
    // Get the index of the flow element track
    size_t FETrackIndex = FE->chargedObjects().at(0)->index();

    std::vector<TauJetLink_t> FETauJetLinks;

    // Loop over the taus
    for (const xAOD::TauJet* tau : *tauChargedFEWriteDecorHandle) {
      // Get tau tracks associated to the tau
      std::vector<const xAOD::TauTrack*> tauTracks = tau->tracks(xAOD::TauJetParameters::coreTrack);
      for (auto tauTrack : tauTracks) {
        // Get track associated to the tau track to use for matching
        const xAOD::TrackParticle* tauIDTrack = tauTrack->track();
        // Get the index of the track associated to the tau
        size_t tauIDTrackIndex = tauIDTrack->index();

        // Link the tau and the charged FE if the track indices match
        if (tauIDTrackIndex == FETrackIndex) {
          FETauJetLinks.push_back( TauJetLink_t(*tauJetReadHandle,tau->index()) );
          tauChargedFEVec.at(tau->index()).push_back( FELink_t(*chargedFEReadHandle, FE->index()) );
        }
      } // end tau track loop
    } // end tau loop

    // Add vector of elements links to the tau jets as a decoration to the FE container
    chargedFETauWriteDecorHandle (*FE) = FETauJetLinks;

  } // end charged FE loop

  ////////////////////////////////////////////
  // Write decorations to TauJet container
  ////////////////////////////////////////////
  // Add vectors of Flow Element (FE) Links as decorations to the TauJet container
  for (const xAOD::TauJet* tau : *tauNeutralFEWriteDecorHandle) {
    tauNeutralFEWriteDecorHandle (*tau) = tauNeutralFEVec.at(tau->index());
    tauChargedFEWriteDecorHandle (*tau) = tauChargedFEVec.at(tau->index());
  }

  ATH_MSG_DEBUG("Execute completed successfully");

  return StatusCode::SUCCESS;
}
