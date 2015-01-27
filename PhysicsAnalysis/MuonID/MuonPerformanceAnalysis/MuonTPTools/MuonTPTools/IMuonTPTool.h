/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IMuonTPTool.h

#ifndef IMuonTPTool_H
#define IMuonTPTool_H

#include "AsgTools/IAsgTool.h"

#include "xAODMuon/MuonContainer.h"
#include "xAODBase/IParticleContainer.h"
#include "MuonPerformanceHistUtils/MuonTPEfficiencyPlots.h"

class IMuonTPTool : virtual public asg::IAsgTool {
  ASG_TOOL_INTERFACE(IMuonTPTool)

public:

  /// run the tag-and-probe 
  virtual void runTagAndProbe(const xAOD::MuonContainer*, const xAOD::IParticleContainer*, const xAOD::IParticleContainer*) const =0;

  /// retrieve booked histograms
  virtual std::vector<HistData> retrieveBookedHistograms() = 0;
  virtual void UpdateEfficiencies() = 0;

};

#endif
