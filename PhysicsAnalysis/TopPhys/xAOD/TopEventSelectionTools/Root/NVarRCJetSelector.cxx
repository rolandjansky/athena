/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#include "TopEventSelectionTools/NVarRCJetSelector.h"
#include "TopEvent/EventTools.h"
#include <iostream>

namespace top {
  NVarRCJetSelector::NVarRCJetSelector(const std::string& name, const std::string& params) :
    SignValueSelector("VRCJET_N " + name, params, true),
    m_name(name) {
    checkMultiplicityIsInteger();
  }

  bool NVarRCJetSelector::apply(const top::Event& event) const {
    auto func = [&](const xAOD::Jet* jetPtr) {
                  return jetPtr->pt() > value();
                };

    top::check(event.m_VarRCJets.find(
                 m_name) != event.m_VarRCJets.end(),
               "Error in NVarRCJetSelector: Variable-R reclustered jets with parameter " + m_name + " not defined!");
    std::unordered_map< std::string, std::shared_ptr<xAOD::JetContainer> > VarRCJets = event.m_VarRCJets;
    auto count = std::count_if(VarRCJets[m_name]->begin(), VarRCJets[m_name]->end(), func);
    return checkInt(count, multiplicity());
  }

  bool NVarRCJetSelector::applyParticleLevel(const top::ParticleLevelEvent& event) const {
    auto func = [&](const xAOD::Jet* jetPtr) {
                  return jetPtr->pt() > value();
                };

    top::check(event.m_VarRCJets.find(
                 m_name) != event.m_VarRCJets.end(),
               "Error in NVarRCJetSelector: Variable-R reclustered jets with parameter " + m_name + " not defined!");
    auto count = std::count_if(event.m_VarRCJets[m_name]->begin(), event.m_VarRCJets[m_name]->end(), func);
    return checkInt(count, multiplicity());
  }
}
