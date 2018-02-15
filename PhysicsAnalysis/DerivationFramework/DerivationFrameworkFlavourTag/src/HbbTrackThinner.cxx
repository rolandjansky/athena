/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// HbbTrackThinner.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: Dan Guest (dguest@cern.ch)
//
// Tool to remove all the tracks in an event except those which are
// associated to to a large R jet over some pt threshold

#include "HbbTrackThinner.h"
#include "AthenaKernel/IThinningSvc.h"
#include "xAODTracking/TrackParticleContainer.h"
#include <vector>
#include <string>

// internal accessor structure
DerivationFramework::HbbAccessors::HbbAccessors():
  parent("Parent"),
  subjets("GhostVR30Rmax4Rmin02TrackJet"),
  coneAssociator("BTagTrackToJetAssociator"),
  ghostAssociator("GhostTrack")
{
}


// Constructor
DerivationFramework::HbbTrackThinner::HbbTrackThinner(
  const std::string& t,
  const std::string& n,
  const IInterface* p ) :
  AthAlgTool(t,n,p),
  m_thinningSvc("ThinningSvc",n),
  m_ntot(0),
  m_npass(0),
  m_largeJetPtCut(200e3),
  m_jetCollectionName("AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets")
{
  declareInterface<DerivationFramework::IThinningTool>(this);
  declareProperty("thinningService", m_thinningSvc);
  declareProperty("largeJetPtCut", m_largeJetPtCut);
  declareProperty("jetCollectionName", m_jetCollectionName);
}

// Destructor
DerivationFramework::HbbTrackThinner::~HbbTrackThinner() {
}

// Athena initialize and finalize
StatusCode DerivationFramework::HbbTrackThinner::initialize()
{
  ATH_MSG_VERBOSE("initialize() ...");
  return StatusCode::SUCCESS;
}
StatusCode DerivationFramework::HbbTrackThinner::finalize()
{
  ATH_MSG_VERBOSE("finalize() ...");
  ATH_MSG_INFO("Processed "<< m_ntot <<" tracks, "<< m_npass<< " were retained ");
  return StatusCode::SUCCESS;
}

// The thinning itself
StatusCode DerivationFramework::HbbTrackThinner::doThinning() const
{

  // build a set of pointers to the particles we want to keep
  std::set<const xAOD::TrackParticle*> jet_tracks;

  // build a list of all the tracks we have in the large R jet
  const xAOD::JetContainer *fatJets = 0;
  CHECK( evtStore()->retrieve(fatJets, m_jetCollectionName) );
  for (const auto* fatJet: *fatJets) {
    if (fatJet->pt() < m_largeJetPtCut || std::abs(fatJet->eta()) > 2.0) {
      continue;
    }
    // get subjets
    const xAOD::Jet* parent_jet = *m_acc.parent(*fatJet);
    if (!parent_jet) throw std::logic_error("no valid parent");
    auto subjet_links = m_acc.subjets(*parent_jet);
    for (const auto& el: subjet_links) {
      const auto* jet = dynamic_cast<const xAOD::Jet*>(*el);
      if (!jet) throw std::logic_error("subjet is invalid");
      // get tracks
      // start with b-tagging cone association
      const xAOD::BTagging *bjet = jet->btagging();
      if (!bjet) {
        ATH_MSG_ERROR("no btagging object found for " + m_jetCollectionName +
                      " skipping");
        return StatusCode::FAILURE;
      }
      const HbbAccessors::TrackLinks trackLinks = m_acc.coneAssociator(*bjet);
      for (const auto& link: trackLinks) {
        const xAOD::TrackParticle* track = *link;
        jet_tracks.insert(track);
      }
      // then use ghosts
      for (const auto& ghost: m_acc.ghostAssociator(*jet)) {
        const auto* track = dynamic_cast<const xAOD::TrackParticle*>(*ghost);
        if (!track) throw std::runtime_error("this isn't a track particle");
        jet_tracks.insert(track);
      }
    }
  }

  // Get the track container
  typedef xAOD::TrackParticleContainer Tracks;
  const auto* tracks = evtStore()->retrieve<const Tracks>("InDetTrackParticles");
  if(!tracks) {
    ATH_MSG_ERROR ("Couldn't find InDetTrackParticles");
    return StatusCode::FAILURE;
  }
  m_ntot += tracks->size();

  // Loop over tracks, see if they pass, set mask
  std::vector<bool> mask;
  for (const xAOD::TrackParticle* track: *tracks) {
    if ( jet_tracks.count(track) ) {
      m_npass++;
      mask.push_back(true);
    } else {
      mask.push_back(false);
    }
  }
  if (m_thinningSvc->filter(*tracks, mask, IThinningSvc::Operator::Or).isFailure()) {
    ATH_MSG_ERROR("Application of thinning service failed! ");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

