/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// IMuonResonancePairingTool.h

#ifndef IMuonResonancePairingTool_H
#define IMuonResonancePairingTool_H

#include "AsgTools/IAsgTool.h"

#include "xAODMuon/MuonContainer.h"
#include "xAODBase/IParticleContainer.h"
//#include "MuonPerformanceHistUtils/ProbeContainer.h"

class IMuonResonancePairingTool : virtual public asg::IAsgTool {
  ASG_TOOL_INTERFACE(IMuonResonancePairingTool)

public:

  //  virtual std::vector<std::pair<xAOD::Muon*, xAOD::Muon*> > buildPairs(std::pair<xAOD::MuonContainer*,xAOD::MuonContainer*>) const =0;
  virtual std::vector<std::pair<const xAOD::Muon*, const xAOD::Muon*> > buildPairs(std::pair<std::vector<const xAOD::Muon*>,std::vector<const xAOD::Muon*> >) const =0;

};

#endif
