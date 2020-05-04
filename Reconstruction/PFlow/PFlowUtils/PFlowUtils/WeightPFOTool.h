/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef WEIGHTPFOTOOL_H
#define WEIGHTPFOTOOL_H

/** Simple class to weight charged PFO for jets and met */

#include "AsgTools/AsgTool.h"

#include "PFlowUtils/IWeightPFOTool.h"

#include <string>

namespace CP {

  class WeightPFOTool : public virtual IWeightPFOTool, public asg::AsgTool {

    /** Athena constructor */
    ASG_TOOL_CLASS( WeightPFOTool, IWeightPFOTool )

  public:
    /** Rootcore constructor */
    WeightPFOTool(const std::string& name);

    // given a PFO, extract weight
    StatusCode fillWeight( const xAOD::PFO& cpfo, float& weight) const;

  private:

    bool m_doEoverPweight;
    std::string m_theNeutralPFOScaleString;

  };

}
#endif
