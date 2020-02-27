/*
 *   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 *   */


#ifndef ITrigEgammaEmulationTool_H
#define ITrigEgammaEmulationTool_H

#include "AsgTools/IAsgTool.h"


namespace Trig{
    class ITrigEgammaEmulationTool : public virtual asg::IAsgTool {
        ASG_TOOL_INTERFACE( Trig::ITrigEgammaEmulationTool )

        public:
            virtual StatusCode initialize(void)=0;

    };
}
#endif
