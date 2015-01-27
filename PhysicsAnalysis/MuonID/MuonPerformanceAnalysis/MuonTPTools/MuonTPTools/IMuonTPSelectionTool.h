/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IMuonTPSelectionTool.h

#ifndef IMuonTPSelectionTool_H
#define IMuonTPSelectionTool_H

#include "AsgTools/IAsgTool.h"

#include "xAODMuon/MuonContainer.h"
#include "xAODBase/IParticleContainer.h"
#include "MuonPerformanceHistUtils/ProbeContainer.h"
#include "MuonPerformanceHistUtils/MuonTPCutFlowBase.h"

class IMuonTPSelectionTool : virtual public asg::IAsgTool {
  ASG_TOOL_INTERFACE(IMuonTPSelectionTool)

public:

  /// Select Probes
  virtual ProbeContainer* selectProbes(const xAOD::MuonContainer*, const xAOD::IParticleContainer*) const =0;

  /// Add a cut flow histogram
  virtual void AddCutFlowHist(MuonTPCutFlowBase* hist) = 0;

  /// Get Efficiency Flag
  virtual std::string efficiencyFlag() =0;

};

#endif
