/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopEventSelectionTools/MV1Selector.h"
#include "TopEvent/EventTools.h"

#include <algorithm>

namespace top {

MV1Selector::MV1Selector(const std::string& params) :
            SignValueSelector("MV1_N", params, true) {
    checkMultiplicityIsInteger();
}

bool MV1Selector::apply(const top::Event& event) const {
    auto func = [&](const xAOD::Jet* const jetPtr) { return jetPtr->btagging()->MV1_discriminant() > value(); };
    auto count = std::count_if(event.m_jets.begin(), event.m_jets.end(), func);
    return checkInt(count, multiplicity());
}

}
