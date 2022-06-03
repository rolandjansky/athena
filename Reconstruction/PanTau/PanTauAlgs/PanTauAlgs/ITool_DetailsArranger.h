/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PANTAUALGS_ITOOL_DETAILSARRANGER_H
#define PANTAUALGS_ITOOL_DETAILSARRANGER_H

#include "AsgTools/IAsgTool.h"

#include "xAODParticleEvent/ParticleContainer.h"

namespace PanTau {
    class PanTauSeed;
}

namespace PanTau {

    /** @class ITool_DetailsArranger
        @brief Interface for PID from tau seeds.

        @author Sebastian Fleischmann
        @author Christian Limbach
    */
    class ITool_DetailsArranger : virtual public asg::IAsgTool {

    ASG_TOOL_INTERFACE(ITool_DetailsArranger)

        public:

        virtual bool isInitialized() = 0;
        virtual StatusCode execute(PanTau::PanTauSeed* inSeed, xAOD::ParticleContainer& pi0Container) const = 0;

    };

}

#endif // PANTAUALGS_ITAUDISCRIMINANTTOOL_H
