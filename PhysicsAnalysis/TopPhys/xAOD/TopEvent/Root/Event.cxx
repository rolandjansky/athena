/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

#include "TopEvent/Event.h"
#include "TopEvent/EventTools.h"

#include "xAODEventInfo/EventInfo.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/ElectronContainerFwd.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODJet/JetContainer.h"
#include "xAODTau/TauJetContainer.h"
#include "xAODMissingET/MissingETContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthVertex.h"
#include "xAODCore/ShallowCopy.h"

#include <iomanip>

std::ostream & operator << (std::ostream& os, const TLorentzVector& v) {
  os << "TLorentzVector" <<
    " pt=" << v.Pt() <<
    " eta " << v.Eta() <<
    " phi " << v.Phi() <<
    " m " << v.M();

  return os;
}

std::ostream& operator << (std::ostream& os, const top::Event& event) {
//     os << *event.m_info << "\n";

  os << "Electrons: " << event.m_electrons.size() << "\n";
  for (const auto* const elPtr : event.m_electrons) {
    os << "    " << *elPtr << "\n";
  }
  os << "FwdElectrons: " << event.m_fwdElectrons.size() << "\n";
  for (const auto* const elPtr : event.m_fwdElectrons) {
    os << "    " << *elPtr << "\n";
  }
  os << "Muons: " << event.m_muons.size() << "\n";
  for (const auto* const muPtr : event.m_muons) {
    os << "    " << *muPtr << "\n";
  }
  os << "Soft Muons: " << event.m_softmuons.size() << "\n";
  for (const auto* const muPtr : event.m_softmuons) {
    os << "    " << *muPtr << "\n";
  }
  os << "Jets: " << event.m_jets.size() << "\n";
  for (const auto* const jetPtr : event.m_jets) {
    os << "    " << *jetPtr << "\n";
  }
  os << "Fail-JVT jets: " << event.m_failJvt_jets.size() << "\n";
  for (const auto* const jetPtr : event.m_failJvt_jets) {
    os << "    " << *jetPtr << "\n";
  }
  os << "Fail-FJVT jets: " << event.m_failFJvt_jets.size() << "\n";
  for (const auto* const jetPtr : event.m_failFJvt_jets) {
    os << "    " << *jetPtr << "\n";
  }
  os << "Large jets: " << event.m_largeJets.size() << "\n";
  for (const auto* const jetPtr : event.m_largeJets) {
    os << "    " << *jetPtr << "\n";
  }
  os << "Track jets: " << event.m_trackJets.size() << "\n";
  for (const auto* const jetPtr : event.m_trackJets) {
    os << "    " << *jetPtr << "\n";
  }
  os << "Tracks: " << event.m_tracks.size() << "\n";
  for (const auto* const trackPtr : event.m_tracks) {
    os << "    " << *trackPtr << "\n";
  }
  os << "Taus: " << event.m_tauJets.size() << "\n";
  for (const auto* const tauPtr : event.m_tauJets) {
    os << "    " << *tauPtr << "\n";
  }
  os << "MET: " << *event.m_met << "\n";

  return os;
}

/*
   std::ostream& operator<<(std::ostream& os, const xAOD::EventInfo& ei) {
    os << "EventInfo" <<
            " isSimulation " << std::boolalpha << ei.eventType(xAOD::EventInfo::IS_SIMULATION) <<
            " runNumber " << ei.runNumber() <<
            " eventNumber " << ei.eventNumber() <<
            " <mu> " << ei.averageInteractionsPerCrossing();

    if (ei.eventType(xAOD::EventInfo::IS_SIMULATION)) {
        os << " mcChannelNumber " << ei.mcChannelNumber() <<
              " mcEventWeight " << ei.mcEventWeight();
    }

    return os;
   }*/

std::ostream& operator << (std::ostream& os, const xAOD::Electron& el) {
  os << "Electron" <<
    " author " << el.author() <<
    " pt " << el.pt() <<
    " eta " << el.eta() <<
    " phi " << el.phi() <<
    " m " << el.m() <<
    " charge " << el.charge();

  return os;
}

std::ostream& operator << (std::ostream& os, const xAOD::Muon& mu) {
  os << "Muon" <<
    " author " << mu.author() <<
    " pt " << mu.pt() <<
    " eta " << mu.eta() <<
    " phi " << mu.phi() <<
    " m " << mu.m() <<
    " charge " << mu.charge();

  return os;
}

std::ostream& operator << (std::ostream& os, const xAOD::Jet& jet) {
  os << "Jet" <<
    " pt " << jet.pt() <<
    " eta " << jet.eta() <<
    " phi " << jet.phi() <<
    " m " << jet.m();

  return os;
}

std::ostream& operator << (std::ostream& os, const xAOD::TauJet& tau) {
  os << "TauJet" <<
    " pt " << tau.pt() <<
    " eta " << tau.eta() <<
    " phi " << tau.phi() <<
    " m " << tau.m() <<
    " charge " << tau.charge();

  return os;
}

std::ostream& operator << (std::ostream& os, const xAOD::TrackParticle& track) {
  os << "Track" <<
    " pt " << track.pt() <<
    " eta " << track.eta() <<
    " phi " << track.phi() <<
    " m " << track.m();

  return os;
}


std::ostream& operator << (std::ostream& os, const xAOD::TruthParticle& truth) {
  const unsigned int w = 10;

  os << "TruthParticle" <<
    " pdg " << std::setw(w) << truth.pdgId() <<
    " status " << std::setw(w) << truth.status() <<
    " pt " << std::setw(w) << truth.pt() <<
    " eta " << std::setw(w) << truth.eta() <<
    " phi " << std::setw(w) << truth.phi() <<
    " e " << std::setw(w) << truth.e() <<
    " m " << std::setw(w) << truth.p4().M();

  return os;
}

std::ostream& operator << (std::ostream& os, const xAOD::MissingET& met) {
  os << "MET" <<
    " et " << met.met() <<
    " phi " << met.phi() << "\n";

  return os;
}
