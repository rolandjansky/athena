/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
// Filename: RecoLevelSelector.h
// Description:
// Author: Fabian Wilk
// Created: Tue Oct 27 12:07:02 2015

#ifndef _RECOLEVELSELECTOR_H_
#define _RECOLEVELSELECTOR_H_

#include "TopEventSelectionTools/EventSelectorBase.h"

namespace top {

    /**
     * @brief An event selector that accepts all reco level events and rejects all
     *  particle level events.
     *
     * This can be used to define event selections that shall be applied only to
     * reco level events.
     */
    class RecoLevelSelector : public EventSelectorBase {
        /**
         * @brief Accept every event.
         * @return Always true.
         */
        virtual bool apply(const top::Event&) const override;

        /**
         * @brief Reject every event.
         * @return Always false.
         */
        virtual bool applyParticleLevel(const top::ParticleLevelEvent&) const override;

        /**
         * @brief The name printed in the cutflow.
         * @return The word RECO_LEVEL
         */
        virtual std::string name() const override;
    };

}

#endif /* _RECOLEVELSELECTOR_H_ */

