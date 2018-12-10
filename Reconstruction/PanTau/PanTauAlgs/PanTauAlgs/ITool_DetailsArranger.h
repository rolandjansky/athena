/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PANTAUALGS_ITOOL_DETAILSARRANGER_H
#define PANTAUALGS_ITOOL_DETAILSARRANGER_H

#include "AsgTools/IAsgTool.h"

#include "xAODParticleEvent/ParticleContainer.h"

//class Jet;
namespace PanTau {
    class PanTauSeed2;
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
        virtual StatusCode execute(PanTau::PanTauSeed2* inSeed, xAOD::ParticleContainer& pi0Container) = 0;

    };

}
#endif // PANTAUALGS_ITAUDISCRIMINANTTOOL_H
