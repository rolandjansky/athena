/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// HbbTrackThinner.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: Dan Guest (dguest@cern.ch)
//
// Tool to remove all the tracks in an event except those which are
// associated to a subjet within a large R jet over some pt threshold

#include "HbbTrackThinner.h"
#include "AthenaKernel/IThinningSvc.h"
#include "xAODTracking/TrackParticleContainer.h"
#include <vector>
#include <string>

namespace DerivationFramework {

// internal accessor structure
  HbbAccessors::HbbAccessors():
    parent("Parent"),
    subjets("GhostVR30Rmax4Rmin02TrackJet"),
    coneAssociator("BTagTrackToJetAssociator"),
    ghostAssociator("GhostTrack")
  {
  }


// Constructor
  HbbTrackThinner::HbbTrackThinner(
    const std::string& t,
    const std::string& n,
    const IInterface* p ) :
    AthAlgTool(t,n,p),
    m_thinningSvc("ThinningSvc",n),
    m_ntot(0),
    m_npass(0),
    m_largeJetPtCut(200e3),
    m_smallJetPtCut(7e3),
    m_nLeadingJets(3),
    m_jetCollectionName("AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets"),
    m_addConeAssociated(false),
    m_addSubjetGhosts(false),
    m_addConstituents(false)
  {
    declareInterface<DerivationFramework::IThinningTool>(this);
    declareProperty("thinningService", m_thinningSvc);
    declareProperty("largeJetPtCut", m_largeJetPtCut);
    declareProperty("smallJetPtCut", m_smallJetPtCut);
    declareProperty("nLeadingJets", m_nLeadingJets);
    declareProperty("jetCollectionName", m_jetCollectionName);
    declareProperty("addConeAssociated", m_addConeAssociated);
    declareProperty("addSubjetGhosts", m_addSubjetGhosts);
    declareProperty("addConstituents", m_addConstituents);
  }

// Destructor
  HbbTrackThinner::~HbbTrackThinner() {
  }

// Athena initialize and finalize
  StatusCode HbbTrackThinner::initialize()
  {
    ATH_MSG_VERBOSE("initialize() ...");
    if (!(m_addConeAssociated || m_addSubjetGhosts || m_addConstituents)) {
      ATH_MSG_ERROR(
        "no association is selected, you will not save any tracks");
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
  }
  StatusCode HbbTrackThinner::finalize()
  {
    ATH_MSG_VERBOSE("finalize() ...");
    ATH_MSG_INFO("Processed "<< m_ntot
                 <<" tracks, "<< m_npass<< " were retained ");
    return StatusCode::SUCCESS;
  }


  /////////////////////////////////////////////////////////////////////
  /// thinning function
  ////////////////////////////////////////////////////////////////////
  //
  StatusCode HbbTrackThinner::doThinning() const
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
      try {
        auto new_tracks = getTracksInJet(*fatJet);
        jet_tracks.insert(new_tracks.begin(), new_tracks.end());
      } catch (std::logic_error& err) {
        ATH_MSG_ERROR(err.what() << " while looking for tracks in" +
                      m_jetCollectionName);
      }
    }

    // Get the track container
    typedef xAOD::TrackParticleContainer Tracks;
    const auto* tracks = evtStore()->retrieve<const Tracks>(
      "InDetTrackParticles");
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
    if (m_thinningSvc->filter(*tracks, mask,
                              IThinningSvc::Operator::Or).isFailure()) {
      ATH_MSG_ERROR("Application of thinning service failed! ");
      return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
  }


  /////////////////////////////////////////////////////////////////////
  /// internal function to get tracks in the jet
  ////////////////////////////////////////////////////////////////////
  //
  std::set<const xAOD::TrackParticle*> HbbTrackThinner::getTracksInJet(
    const xAOD::Jet& fatJet) const {
    using xAOD::Jet;
    std::set<const xAOD::TrackParticle*> jet_tracks;
    // get subjets
    const Jet* parent_jet = *m_acc.parent(fatJet);
    if (!parent_jet) throw std::logic_error("no valid parent jet");
    auto subjet_links = m_acc.subjets(*parent_jet);
    std::vector<const Jet*> subjets;
    for (const auto& el: subjet_links) {
      const auto* jet = dynamic_cast<const Jet*>(*el);
      if (!jet) throw std::logic_error("subjet is invalid");
      if (subjets.size() < m_nLeadingJets &&
          jet->pt() >= m_smallJetPtCut) {
        subjets.push_back(jet);
      }
    }
    std::sort(subjets.begin(), subjets.end(),
              [](const Jet* j1, const Jet* j2){return j1->pt() > j2->pt();});
    for (const Jet* jet: subjets) {
      // get tracks
      // start with b-tagging cone association
      const xAOD::BTagging *bjet = jet->btagging();
      if (!bjet) {
        throw std::logic_error("no btagging object: " + m_jetCollectionName);
      }

      // various ways to add tracks
      if (m_addConeAssociated) {
        const auto trackLinks = m_acc.coneAssociator(*bjet);
        for (const auto& link: trackLinks) {
          const xAOD::TrackParticle* track = *link;
          jet_tracks.insert(track);
        }
      }
      if (m_addSubjetGhosts) {
        for (const auto& ghost: m_acc.ghostAssociator(*jet)) {
          const auto* track = dynamic_cast<const xAOD::TrackParticle*>(*ghost);
          if (!track) throw std::runtime_error("this isn't a track particle");
          jet_tracks.insert(track);
        }
      }
      if (m_addConstituents) {
        for (const auto& link: jet->constituentLinks()) {
          const auto* track = dynamic_cast<const xAOD::TrackParticle*>(*link);
          if (!track) {
            throw std::logic_error("constituent isn't a track particle");
          }
          jet_tracks.insert(track);
        }
      }
    }
    return jet_tracks;
  }

}
