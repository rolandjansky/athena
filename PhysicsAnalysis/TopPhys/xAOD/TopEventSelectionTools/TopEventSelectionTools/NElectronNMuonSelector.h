/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef NELECTRONNMUONSELECTOR_H_
#define NELECTRONNMUONSELECTOR_H_

#include "TopEventSelectionTools/SignValueSelector.h"

namespace top {

/**
 * @brief To accept an event based on a check of electrons and muons.
 */
class NElectronNMuonSelector : public SignValueSelector {
public:
    /**
     * @brief Select events based on multiplicity, which should be an integer.
     */
    explicit NElectronNMuonSelector(const std::string& params);

    /**
     * @brief The 'OR' of a check against electrons and muons.
     *
     * @return True to keep the event, false otherwise.
     */
    bool apply(const top::Event& event) const override;

    /*!
      * @brief The 'OR' of a check against electrons and muons.
     *
     * @return True to keep the event, false otherwise.
     */
    bool applyParticleLevel(const top::ParticleLevelEvent& event) const override;
};

}

#endif
