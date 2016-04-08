/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IRETRIEVEPFOTOOL_H
#define IRETRIEVEPFOTOOL_H

/** Simple class to retrieve PFO for jets and met in the two possible configurations we provide */

#include "AsgTools/IAsgTool.h"

#include "xAODPFlow/PFOContainer.h"
#include "PFlowUtils/PFODefs.h"

namespace CP {

  class IRetrievePFOTool : public virtual asg::IAsgTool {

    /** Declare the interface that the class provides */
    ASG_TOOL_INTERFACE( CP::IRetrievePFOTool )
      
    public:

    /** Fills theContainer with PFO from EM or LC mode in eflowRec */
    virtual const xAOD::PFOContainer* retrievePFO(const CP::PFO_JetMETConfig_inputScale& theScale) = 0;
    
     /** Fills theContainer with PFO from EM or LC mode in eflowRec - additionally allows to choose one of charged, neutral or all PFO configurations */
    virtual const xAOD::PFOContainer* retrievePFO(const CP::PFO_JetMETConfig_inputScale& theScale, const CP::PFO_JetMETConfig_charge& theCharge) = 0;

  };

}
#endif
