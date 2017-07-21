/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopEventSelectionTools/MV2c20Selector.h"
#include "TopEvent/EventTools.h"

#include <algorithm>

namespace top {

MV2c20Selector::MV2c20Selector(const std::string& params) :
            SignValueSelector("MV2C20_N", params, true) {
    checkMultiplicityIsInteger();
}

bool MV2c20Selector::apply(const top::Event& event) const {
    auto func = [&](const xAOD::Jet* const jetPtr) {
        double mv2c20_discriminant = 0.;
        const bool hasMv2c20 = jetPtr->btagging()->MVx_discriminant("MV2c20", mv2c20_discriminant);
        if (hasMv2c20)
            return mv2c20_discriminant > value();

        return false;
    };

    auto count = std::count_if(event.m_jets.begin(), event.m_jets.end(), func);
    return checkInt(count, multiplicity());
}

}
