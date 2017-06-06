/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MLLSELECTOR_H_
#define MLLSELECTOR_H_

#include "TopEventSelectionTools/SignValueSelector.h"

namespace top {

/**
 * @brief Put a cut on the invariant mass of two same flavour leptons.  If you
 * want to veto a window (say for the Z mass) you instead want MLLWindow.
 */
class MLLSelector : public SignValueSelector {
public:
    explicit MLLSelector(const std::string& params);

    bool apply(const top::Event& event) const override;

    bool applyParticleLevel(const top::ParticleLevelEvent& event) const override;
};

}

#endif
