/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MLLWINDOW_H_
#define MLLWINDOW_H_

#include "TopEventSelectionTools/SignValueSelector.h"

namespace top {

/**
 * @brief Reject events in the invariant mass (of two charged leptons) window.
 */
class MLLWindow : public EventSelectorBase {
public:
    explicit MLLWindow(std::string params);

    bool apply(const top::Event& event) const override;

    bool applyParticleLevel(const top::ParticleLevelEvent& event) const override;

    std::string name() const override;

private:
    double m_cutvalue0;
    double m_cutvalue1;
};

}

#endif
