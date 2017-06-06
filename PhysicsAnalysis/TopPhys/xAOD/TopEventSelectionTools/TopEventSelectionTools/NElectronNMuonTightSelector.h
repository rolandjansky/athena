/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef NELECTRONNMUONTIGHTSELECTOR_H_
#define NELECTRONNMUONTIGHTSELECTOR_H_

#include "TopEventSelectionTools/SignValueSelector.h"

namespace top {

/**
 * @brief To accept an event based on a check of tight electrons and muons.
 * This is identical to NElectronNMuonSelector in the case of tight selection, so only relevant for loose selection.
 */
class NElectronNMuonTightSelector : public SignValueSelector {
public:
    /**
     * @brief Select events based on multiplicity, which should be an integer.
     */
    explicit NElectronNMuonTightSelector(const std::string& params);

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
