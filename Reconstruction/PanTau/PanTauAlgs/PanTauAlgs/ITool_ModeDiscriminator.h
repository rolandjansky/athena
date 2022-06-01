/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PANTAUALGS_ITOOL_MODEDISCRIMINATOR_H
#define PANTAUALGS_ITOOL_MODEDISCRIMINATOR_H

#include "AsgTools/IAsgTool.h"

namespace PanTau {
    class PanTauSeed;
}

namespace PanTau {

    /** @class ITool_ModeDiscriminator
        @brief Interface for PID from tau seeds.

        @author Sebastian Fleischmann
        @author Christian Limbach
    */
    class ITool_ModeDiscriminator : virtual public asg::IAsgTool {

    ASG_TOOL_INTERFACE(ITool_ModeDiscriminator)

        public:

    virtual bool isInitialized() = 0;
    virtual double getResponse(PanTau::PanTauSeed* inSeed, bool& isOK) const = 0;
    };

}
#endif // PANTAUALGS_ITAUDISCRIMINANTTOOL_H
