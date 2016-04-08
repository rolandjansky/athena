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
  
  // check if the tool represents a nominal run or a sys variation
  virtual bool isNominal() const=0;
  
  // check if the tool is a systematic variation that can not be evaluated using the nominal ntuples
  virtual bool notIncludedInNominal() const=0;
  
  virtual std::vector<std::string> tagTriggerList() const = 0;

};

#endif
