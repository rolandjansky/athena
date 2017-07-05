/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopEventSelectionTools/NTauTightSelector.h"

namespace top {

NTauTightSelector::NTauTightSelector(const std::string& params) :
            SignValueSelector("TAU_N_TIGHT", params, true) {
    checkMultiplicityIsInteger();
}

bool NTauTightSelector::apply(const top::Event& event) const {
    auto func = [&](const xAOD::TauJet* tauPtr){return tauPtr->pt() > value() && tauPtr->auxdataConst<char>("passPreORSelection");};
    auto count = std::count_if(event.m_tauJets.begin(), event.m_tauJets.end(), func);
    return checkInt(count, multiplicity());
}

}
