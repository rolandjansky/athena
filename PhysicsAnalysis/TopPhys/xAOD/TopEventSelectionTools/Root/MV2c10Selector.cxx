/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#include "TopEventSelectionTools/MV2c10Selector.h"
#include "TopEvent/EventTools.h"
#include "xAODBTagging/BTaggingUtilities.h"

#include <algorithm>

namespace top {
  MV2c10Selector::MV2c10Selector(const std::string& params) :
    SignValueSelector("MV2C10_N", params, true) {
    checkMultiplicityIsInteger();
  }

  bool MV2c10Selector::apply(const top::Event& event) const {
    auto func = [&](const xAOD::Jet* const jetPtr) {
                  double mv2c10_discriminant = 0.;
                  const bool hasMv2c10 = xAOD::BTaggingUtilities::getBTagging(*jetPtr)->MVx_discriminant("MV2c10", mv2c10_discriminant);

                  if (hasMv2c10) return mv2c10_discriminant > value();

                  return false;
                };

    auto count = std::count_if(event.m_jets.begin(), event.m_jets.end(), func);

    return checkInt(count, multiplicity());
  }
}
