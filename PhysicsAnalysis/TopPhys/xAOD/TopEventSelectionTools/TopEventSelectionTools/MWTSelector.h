/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MWTSELECTOR_H_
#define MWTSELECTOR_H_

#include "TopEventSelectionTools/SignValueSelector.h"

namespace top {

/**
 * @brief Apply a cut on the transverse W mass (taken from the electron or muon
 * and MET).
 */
class MWTSelector : public SignValueSelector {
public:
    explicit MWTSelector(const std::string& params);

    bool apply(const top::Event& event) const override;

    bool applyParticleLevel(const top::ParticleLevelEvent& event) const override;
};

}

#endif
