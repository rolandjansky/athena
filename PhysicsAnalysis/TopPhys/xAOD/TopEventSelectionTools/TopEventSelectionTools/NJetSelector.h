/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef NJETSELECTOR_H_
#define NJETSELECTOR_H_

#include "TopEventSelectionTools/SignValueSelector.h"

namespace top {

/**
 * @brief A tool that selects events with a certain number of good jets.
 */
class NJetSelector : public SignValueSelector {
public:
    explicit NJetSelector(const std::string& params);

    bool apply(const top::Event& event) const override;

    bool applyParticleLevel(const top::ParticleLevelEvent& event) const override;
};

}

#endif
