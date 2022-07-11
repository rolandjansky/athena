/*
 * Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
 */

#ifndef TRIGGERMATCHINGTOOL_IMATCHSCORINGTOOL_H
#define TRIGGERMATCHINGTOOL_IMATCHSCORINGTOOL_H

#include "AsgTools/IAsgTool.h"

namespace xAOD {
    class IParticle;
}

namespace Trig {
    /// Interface class for scoring pairs of particles for matching
    class IMatchScoringTool : virtual public asg::IAsgTool {
    public:
        /**
         * @brief Calculate the matching score for a pair of particles
         * @param online The online particle
         * @param offline The offline particle
         */
        virtual float score(
                const xAOD::IParticle &online,
                const xAOD::IParticle &offline) const = 0;
    }; //> end class IMatchScoringTool
} //> end namespace Trig

#endif //> !TRIGGERMATCHINGTOOL_IMATCHSCORINGTOOL_H
