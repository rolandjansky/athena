/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopEventSelectionTools/IP3DSV1Selector.h"

#include <algorithm>

namespace top {

IP3DSV1Selector::IP3DSV1Selector(const std::string& params) :
    SignValueSelector("IP3DSV1_N", params, true) {
    checkMultiplicityIsInteger();
}

bool IP3DSV1Selector::apply(const top::Event& event) const {
    auto func = [&](const xAOD::Jet* const jetPtr) { return jetPtr->btagging()->SV1plusIP3D_discriminant() > value(); };
    auto count = std::count_if(event.m_jets.begin(), event.m_jets.end(), func);
    return checkInt(count, multiplicity());
}

}
