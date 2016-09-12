/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IWEIGHTPFOTOOL_H
#define IWEIGHTPFOTOOL_H

/** Simple class to retrieve PFO for jets and met in the two possible configurations we provide */

#include "AsgTools/IAsgTool.h"

#include "xAODPFlow/PFOContainer.h"
#include "PFlowUtils/PFODefs.h"

namespace CP {

  class IWeightPFOTool : public virtual asg::IAsgTool {

    /** Declare the interface that the class provides */
    ASG_TOOL_INTERFACE( CP::IWeightPFOTool )
      
    public:

    /** given a PFO, extract weight */
    virtual StatusCode fillWeight( const xAOD::PFO& cpfo, float& weight ) = 0;

  };

}
#endif
