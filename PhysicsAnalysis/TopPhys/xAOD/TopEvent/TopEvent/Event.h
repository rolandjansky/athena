/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

#ifndef TOPEVENT_H_
#define TOPEVENT_H_

#include "xAODEventInfo/EventInfo.h"
#include "xAODTracking/VertexContainer.h"

#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/ElectronContainerFwd.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODJet/JetContainer.h"
#include "xAODTau/TauJetContainer.h"
#include "xAODMissingET/MissingET.h"

#include "xAODTruth/TruthEventContainer.h"
#include "xAODTruth/TruthParticleContainer.h"

#include "TopPartons/PartonHistory.h"
#include "TopEvent/KLFitterResult.h"
#include "TopEvent/PseudoTopResult.h"

#include <ostream>
#include <iostream>


namespace xAOD {
  class SystematicEvent;
}

/**
 * @brief A namespace for the top group, to avoid using the same name as
 * somewhere else in the code.
 */
namespace top {
/**
 * @brief Very simple class to hold event data after reading from a file.
 *
 * The idea of this is that we read from the xAOD and provide links here to all
 * of the information the user might need.  The code can add / remove objects
 * and plot things from this.
 *
 * The best way to ensure this is reset every event is probably to make a new
 * one every event, that way it'll tidy-up after itself.
 */
  class Event {
  public:
    Event() :
      m_hashValue(0),      // this will be set properly by TopEventMaker
      m_ttreeIndex(99999), // this will be set properly by TopEventMaker
      m_isLoose(false),    // this will be set properly by TopEventMaker
      m_saveEvent(false),
      m_info(nullptr),
      m_primaryVertices(nullptr),
      m_electrons(SG::VIEW_ELEMENTS),
      m_fwdElectrons(SG::VIEW_ELEMENTS),
      m_muons(SG::VIEW_ELEMENTS),
      m_softmuons(SG::VIEW_ELEMENTS),
      m_jets(SG::VIEW_ELEMENTS),
      m_failJvt_jets(SG::VIEW_ELEMENTS),
      m_failFJvt_jets(SG::VIEW_ELEMENTS),
      m_photons(SG::VIEW_ELEMENTS),
      m_largeJets(SG::VIEW_ELEMENTS),
      m_RCJets(SG::VIEW_ELEMENTS),
      m_trackJets(SG::VIEW_ELEMENTS),
      m_tracks(SG::VIEW_ELEMENTS),
      m_tauJets(SG::VIEW_ELEMENTS),
      m_met(nullptr),

      m_truthEvent(nullptr),
      m_truth(nullptr),
      m_partonHistory(nullptr),
      m_KLFitterResults(nullptr),
      m_PseudoTopResult(nullptr) {
    }

    ///systematic has value
    std::size_t m_hashValue;
    ///TTree index
    unsigned int m_ttreeIndex;
    ///is Loose Event?
    bool m_isLoose;
    ///save the event?
    bool m_saveEvent;

    ///Pointer to the event info - nullptr if not loaded, but that's probably a bad idea
    const xAOD::EventInfo* m_info;

    ///Pointer to the systematic event info - nullptr if not loaded, but that's probably a bad idea
    xAOD::SystematicEvent const* m_systematicEvent;

    ///Primary Vertices
    const xAOD::VertexContainer* m_primaryVertices;

    ///Container of electrons (can be sorted)
    xAOD::ElectronContainer m_electrons;

    ///Container of forward electrons (can be sorted)
    xAOD::ElectronContainer m_fwdElectrons;

    ///Container of muons (can be sorted)
    xAOD::MuonContainer m_muons;

    ///Container of soft muons (can be sorted)
    xAOD::MuonContainer m_softmuons;

    ///Container of jets (can be sorted)
    xAOD::JetContainer m_jets;

    ///Container of fail-JVT jets (can be sorted)
    xAOD::JetContainer m_failJvt_jets;

    ///Container of fail-FJVT jets (can be sorted)
    xAOD::JetContainer m_failFJvt_jets;

    ///Container of photons (can be sorted)
    xAOD::PhotonContainer m_photons;

    ///Container of large jets (can be sorted)
    xAOD::JetContainer m_largeJets;

    ///Container of recluster jets (can be sorted)
    xAOD::JetContainer m_RCJets;

    /// Containers of variable-R reclustered jets (can be sorted)
    mutable std::unordered_map< std::string, std::shared_ptr<xAOD::JetContainer> > m_VarRCJets;

    ///Container of track jets (can be sorted)
    xAOD::JetContainer m_trackJets;

    ///Container of tracks (can be sorted)                                                                                                                                                                
    xAOD::TrackParticleContainer m_tracks;

    ///Container of taujets (can be sorted)
    xAOD::TauJetContainer m_tauJets;

    ///Pointer to MET if it is loaded
    const xAOD::MissingET* m_met;

    ///Truth event
    const xAOD::TruthEventContainer* m_truthEvent;

    ///Container of truth particles
    const xAOD::TruthParticleContainer* m_truth;

    ///Top Parton TopPartonHistory
    const xAOD::PartonHistory* m_partonHistory;

    ///KLFitter Results
    mutable const xAOD::KLFitterResultContainer* m_KLFitterResults;

    ///Pseudo Top Results
    mutable const xAOD::PseudoTopResultContainer* m_PseudoTopResult;

    ///JVT and fJVT SFs - now needed here because they include jets that are good jets,
    ///but which are not in the top::Event::m_jets container
    float m_jvtSF = 1.;
    float m_jvtSF_UP = 1.;
    float m_jvtSF_DOWN = 1.;
    float m_fjvtSF = 1.;
    float m_fjvtSF_UP = 1.;
    float m_fjvtSF_DOWN = 1.;
  };
}

std::ostream& operator << (std::ostream& os, const TLorentzVector& v);
std::ostream& operator << (std::ostream& os, const top::Event& event);
std::ostream& operator << (std::ostream& os, const xAOD::EventInfo& ei);
std::ostream& operator << (std::ostream& os, const xAOD::Electron& el);
std::ostream& operator << (std::ostream& os, const xAOD::Muon& mu);
std::ostream& operator << (std::ostream& os, const xAOD::Photon& ph);
std::ostream& operator << (std::ostream& os, const xAOD::Jet& jet);
std::ostream& operator << (std::ostream& os, const xAOD::TauJet& tau);
std::ostream& operator << (std::ostream& os, const xAOD::TruthParticle& truth);
std::ostream& operator << (std::ostream& os, const xAOD::MissingET& met);
std::ostream& operator << (std::ostream& os, const xAOD::TrackParticle& track);

#endif
