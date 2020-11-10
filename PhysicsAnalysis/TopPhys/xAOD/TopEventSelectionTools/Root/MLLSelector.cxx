/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#include "TopEventSelectionTools/MLLSelector.h"
#include "TopEvent/EventTools.h"

namespace top {
  MLLSelector::MLLSelector(const std::string& params) :
    SignValueSelector("MLL", params) {
  }

  bool MLLSelector::apply(const top::Event& event) const {
    const xAOD::IParticle* lepton0 = 0;
    const xAOD::IParticle* lepton1 = 0;

    if (event.m_electrons.size() == 2) {
      lepton0 = event.m_electrons[0];
      lepton1 = event.m_electrons[1];
    } else if (event.m_muons.size() == 2) {
      lepton0 = event.m_muons[0];
      lepton1 = event.m_muons[1];
    } else if (event.m_electrons.size() == 1 && event.m_muons.size() == 1) {
      lepton0 = event.m_electrons[0];
      lepton1 = event.m_muons[0];
    } else {
      throw std::runtime_error("MLLSelector::apply: Need two charge leptons");
    }

    const double mll = top::invariantMass(*lepton0, *lepton1);
    return checkFloat(mll, value());
  }

  bool MLLSelector::applyParticleLevel(const top::ParticleLevelEvent& event) const {
    // If any of the required collections is a nullptr (i.e. has not been
    // loaded) return false.
    if (not event.m_electrons
        or not event.m_muons) {
      return false;
    }

    const xAOD::IParticle* lepton0 = 0;
    const xAOD::IParticle* lepton1 = 0;

    if (event.m_electrons->size() == 2) {
      lepton0 = (*event.m_electrons)[0];
      lepton1 = (*event.m_electrons)[1];
    } else if (event.m_muons->size() == 2) {
      lepton0 = (*event.m_muons)[0];
      lepton1 = (*event.m_muons)[1];
    } else if (event.m_electrons->size() == 1 && event.m_muons->size() == 1) {
      lepton0 = (*event.m_electrons)[0];
      lepton1 = (*event.m_muons)[0];
    } else {
      throw std::runtime_error("MLLSelector::applyParticleLevel: Need two charged leptons");
    }

    const double mll = top::invariantMass(*lepton0, *lepton1);
    return checkFloat(mll, value());
  }
}
