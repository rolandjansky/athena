/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopEventSelectionTools/NTauSelector.h"

namespace top {

NTauSelector::NTauSelector(const std::string& params) :
            SignValueSelector("TAU_N", params, true) {
    checkMultiplicityIsInteger();
}

bool NTauSelector::apply(const top::Event& event) const {
    auto func = [&](const xAOD::TauJet* tauPtr){return tauPtr->pt() > value();};
    auto count = std::count_if(event.m_tauJets.begin(), event.m_tauJets.end(), func);
    return checkInt(count, multiplicity());
}

}
