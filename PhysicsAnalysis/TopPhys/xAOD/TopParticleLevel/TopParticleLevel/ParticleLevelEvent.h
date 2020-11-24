/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

// Filename: ParticleLevelEvent.h
// Description:
// Author: Fabian Wilk
// Created: Tue Feb 24 17:54:49 2015

#ifndef _TOP_PARTICLELEVELEVENT_H_
#define _TOP_PARTICLELEVELEVENT_H_

#include <string>
#include <unordered_map>
#include <iostream>

#include "xAODEventInfo/EventInfo.h"
#include "xAODJet/JetContainer.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODMissingET/MissingET.h"
#include "TopEvent/Event.h"

namespace top {
  class ParticleLevelEvent {
  public:
    ParticleLevelEvent()
      : m_info(nullptr),
      m_electrons(nullptr),
      m_muons(nullptr),
      m_photons(nullptr),
      m_jets(nullptr),
      m_RCJets(SG::VIEW_ELEMENTS),
      m_largeRJets(nullptr),
      m_met(nullptr),
      m_selectionDecisions() {}


    /// Pointer to the event info object
    const xAOD::EventInfo* m_info;

    /// Pointer to truth level electrons
    const xAOD::TruthParticleContainer* m_electrons;

    /// Pointer to truth level muons
    const xAOD::TruthParticleContainer* m_muons;
    
    /// Pointer to truth level soft-muons
    const xAOD::TruthParticleContainer* m_softmuons;

    /// Pointer to truth level photons
    const xAOD::TruthParticleContainer* m_photons;

    /// Pointer to truth level jets
    const xAOD::JetContainer* m_jets;

    ///Container of recluster jets (can be sorted)
    xAOD::JetContainer m_RCJets;

    /// Containers of variable-R reclustered jets (can be sorted)
    mutable std::unordered_map< std::string, std::shared_ptr<xAOD::JetContainer> > m_VarRCJets;

    /// Pointer to the truth level large R jets.
    const xAOD::JetContainer* m_largeRJets;

    /// Pointer to truth level photons
    const xAOD::TruthParticleContainer* m_taus;

    // Pointer to truth level MET
    const xAOD::MissingET* m_met;

    // Maps from the name of an event selection line to the result of
    // applying that event selection onto this ParticleLevelEvent. At the
    // moment, the mapped value (albeit being of type 'int') is inteded just
    // as a true / false statement. Must be mutable because the
    // EventSelectionManager will modify it.
    mutable std::unordered_map<std::string, int> m_selectionDecisions;
  };
}

/// Ostream overload operator
std::ostream& operator << (std::ostream& os, const top::ParticleLevelEvent& plEvent);

#endif /* _TOP_PARTICLELEVELEVENT_H_ */
