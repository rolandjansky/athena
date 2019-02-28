/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TopParticleLevel/ParticleLevelEvent.h"

std::ostream& operator<<(std::ostream& os, const top::ParticleLevelEvent& plEvent) {

  if (plEvent.m_electrons) {
    os << "Number of electrons: " << plEvent.m_electrons->size() << "\n";
    for (const auto& elPtr : *plEvent.m_electrons) {
      if (elPtr) {
        os << " " << *elPtr << "\n";
      }
    }
  } else {
    os << "ParticleLevelEvent: Cannot find electron truth collection. Did you set the truth collection correctly?\n";
  }

  if (plEvent.m_muons) {
    os << "Number of muons: " << plEvent.m_muons->size() << "\n";
    for (const auto& muPtr : *plEvent.m_muons) {
      if (muPtr) {
        os << " " << *muPtr << "\n";
      }
    }
  } else {
    os << "ParticleLevelEvent: Cannot find muon truth collection. Did you set the truth collection correctly?\n";
  }

  if (plEvent.m_jets) {
    os << "Number of jets: " << plEvent.m_jets->size() << "\n";
    for (const auto& jetPtr : *plEvent.m_jets) {
      if (jetPtr) {
        os << " " << *jetPtr << "\n";
      }
    }
  } else {
    os << "ParticleLevelEvent: Cannot find jet truth collection. Did you set the truth collection correctly?\n";
  }

  if (plEvent.m_largeRJets) {
    os << "Number of large R jets: " << plEvent.m_largeRJets->size() << "\n";
    for (const auto& largeRjetPtr : *plEvent.m_largeRJets) {
      if (largeRjetPtr) {
        os << " " << *largeRjetPtr << "\n";
      }
    }
  } else {
    os << "ParticleLevelEvent: Cannot find large R jets truth collection. Did you set the truth collection correctly?\n";
  }

  if (plEvent.m_photons) {
    os << "Number of photons: " << plEvent.m_photons->size() << "\n";
    for (const auto& phPtr : *plEvent.m_photons) {
      if (phPtr) {
        os << " " << *phPtr << "\n";
      }
    }
  } else {
    os << "ParticleLevelEvent: Cannot find photon truth collection. Did you set the truth collection correctly?\n";
  }

  if (plEvent.m_met){
    os << *plEvent.m_met;
  } else {
    os << "ParticleLevelEvent: Cannot find MET (neutrino) truth collection. Did you set the truth collection correctly?\n";
  }

  return os;
}
