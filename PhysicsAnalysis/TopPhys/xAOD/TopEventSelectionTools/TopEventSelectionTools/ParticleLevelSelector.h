/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
// Filename: ParticleLevelSelector.h
// Description:
// Author: Fabian Wilk
// Created: Tue Oct 27 13:26:19 2015

#ifndef _PARTICLELEVELSELECTOR_H_
#define _PARTICLELEVELSELECTOR_H_

#include "TopEventSelectionTools/EventSelectorBase.h"

namespace top {

    /**
     * @brief An event selector that accepts all particle level events and rejects all
     *  reco level events.
     *
     * This can be used to define event selections that shall be applied only to
     * particle level events.
     */
    class ParticleLevelSelector : public EventSelectorBase {
        /**
         * @brief Reject every event.
         * @return Always false.
         */
        virtual bool apply(const top::Event&) const override;

        /**
         * @brief Accept every event.
         * @return Always true.
         */
        virtual bool applyParticleLevel(const top::ParticleLevelEvent&) const override;

        /**
         * @brief The name printed in the cutflow.
         * @return The word PARTICLE_LEVEL
         */
        virtual std::string name() const override;
    };

}

#endif /* _PARTICLELEVELSELECTOR_H_ */

