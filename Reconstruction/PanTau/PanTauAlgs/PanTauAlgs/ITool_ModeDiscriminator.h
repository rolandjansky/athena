/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PANTAUALGS_ITOOL_MODEDISCRIMINATOR_H
#define PANTAUALGS_ITOOL_MODEDISCRIMINATOR_H

#include "AsgTools/IAsgTool.h"


//class Jet;
namespace PanTau {
    class PanTauSeed2;
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
            //virtual double getModeLikeliness(PanTau::PanTauSeed2* inSeed, bool& wasSuccessful) = 0;
            virtual double getResponse(PanTau::PanTauSeed2* inSeed, bool& isOK) = 0;
    };

}
#endif // PANTAUALGS_ITAUDISCRIMINANTTOOL_H
