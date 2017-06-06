/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HTSELECTOR_H_
#define HTSELECTOR_H_

#include "TopEventSelectionTools/SignValueSelector.h"

namespace top {

/**
 * @brief Apply a cut on the event HT.
 *
 * The calculation is done by top::ht().
 */
class HTSelector : public SignValueSelector {
public:
    explicit HTSelector(const std::string& params);

    virtual bool apply(const top::Event& event) const override;

    virtual bool applyParticleLevel(const top::ParticleLevelEvent& event) const override;
};

}

#endif
