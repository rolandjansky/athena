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
    ASG_TOOL_CLASS( WeightPFOTool, IWeightPFOTool );

  public:
    /** Rootcore constructor */
    WeightPFOTool(const std::string& name);

    // given a PFO, extract weight and (potentially) modified TLV
    StatusCode fillWeightAndP4( const xAOD::PFO& pcpf, float& weight, TLorentzVector& cpfo_p4 );

  private:

  };

}
#endif
