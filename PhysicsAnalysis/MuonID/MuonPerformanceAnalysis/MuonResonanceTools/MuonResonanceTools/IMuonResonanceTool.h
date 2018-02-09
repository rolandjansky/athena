/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IMuonResonanceTool.h

#ifndef IMuonResonanceTool_H
#define IMuonResonanceTool_H

#include "AsgTools/IAsgTool.h"

#include "xAODMuon/MuonContainer.h"
#include "xAODBase/IParticleContainer.h"
#include "MuonPerformanceHistUtils/MuonResonancePlots.h"
#include "PATInterfaces/SystematicVariation.h"
#include "PATInterfaces/SystematicRegistry.h"
#include "PATInterfaces/SystematicCode.h"

class IMuonResonanceTool : virtual public asg::IAsgTool {
  ASG_TOOL_INTERFACE(IMuonResonanceTool)

    public:

  virtual void findResonance(const xAOD::MuonContainer*, bool isMC) const =0;

  virtual std::vector<HistData> retrieveBookedHistograms() = 0;

};

#endif
