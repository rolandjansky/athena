/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IMuonTPEfficiencyTool.h

#ifndef IMuonTPEfficiencyTool_H
#define IMuonTPEfficiencyTool_H

#include "AsgTools/IAsgTool.h"

#include "xAODBase/IParticleContainer.h"
#include "MuonPerformanceHistUtils/ProbeContainer.h"
#include "MuonPerformanceHistUtils/MuonTPEfficiencyPlots.h"

class IMuonTPEfficiencyTool : virtual public asg::IAsgTool {
  ASG_TOOL_INTERFACE(IMuonTPEfficiencyTool)

public:

  /// Match probes for efficiency calculation
  virtual void matchProbes(ProbeContainer*, const xAOD::IParticleContainer*) const =0;

  /// Get Efficiency Flag
  virtual std::string efficiencyFlag() =0;
  
  // check if the tool represents a nominal run or a sys variation
  virtual bool isNominal() const=0;

  // return the triger item (if any configured)
  virtual std::string triggerItem() =0;  
};

#endif
