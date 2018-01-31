/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RETRIEVEPFOTOOL_H
#define RETRIEVEPFOTOOL_H

/** Simple class to retrieve PFO for jets and met in the two possible configurations we provide */

#include "AsgTools/AsgTool.h"

#include "PFlowUtils/IRetrievePFOTool.h"

#include <string>

namespace CP {

  class RetrievePFOTool : public virtual IRetrievePFOTool, public asg::AsgTool {

    /** Athena constructor */
    ASG_TOOL_CLASS( RetrievePFOTool, IRetrievePFOTool )

  public:
    /** Rootcore constructor */
    RetrievePFOTool(const std::string& name);

    /** Fills theContainer with PFO from EM or LC mode in eflowRec - the client owns theContainer */
    virtual const xAOD::PFOContainer* retrievePFO(const CP::PFO_JetMETConfig_inputScale& theScale) const override;

    /** Fills theContainer with PFO from EM or LC mode in eflowRec - additionally allows to choose one of charged, neutral or all PFO configurations - the client owns theContainer */
    virtual const xAOD::PFOContainer* retrievePFO(const CP::PFO_JetMETConfig_inputScale& theScale, const CP::PFO_JetMETConfig_charge& theCharge) const override; 

  private:

    /** Fills theContainer with neutral PFO from EM or LC mode in eflowRec */
    StatusCode retrieve_neutralPFO(const CP::PFO_JetMETConfig_inputScale& theScale, xAOD::PFOContainer* theContainer) const;

    /** This retrieves a PFO container with theName and adds the PFO* to newContainer */
    StatusCode fillPFOContainer( xAOD::PFOContainer* newContainer, const std::string& theName) const;

  };

}
#endif
