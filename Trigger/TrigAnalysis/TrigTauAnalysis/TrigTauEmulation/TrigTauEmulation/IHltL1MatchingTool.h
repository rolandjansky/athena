/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// vim: ts=2 sw=2
#ifndef IHLTL1MATCHING_HLTL1MATCHING_H
#define IHLTL1MATCHING_HLTL1MATCHING_H

#include "AsgTools/IAsgTool.h"

// EDM
#include "xAODTau/TauJetContainer.h"
#include "xAODTrigger/EmTauRoIContainer.h"

#include "TrigTauEmulation/DecoratedHltTau.h"

class IHltL1MatchingTool : public virtual asg::IAsgTool

{
  ASG_TOOL_INTERFACE(IHltL1MatchingTool)

 public:

  virtual ~IHltL1MatchingTool() {};

  virtual StatusCode execute(const xAOD::TauJetContainer* hlt_taus, 
			     const xAOD::EmTauRoIContainer* l1taus) = 0;
  virtual StatusCode execute(const std::vector<DecoratedHltTau>& hlt_taus, const xAOD::EmTauRoIContainer * l1_taus) = 0;


};
#endif
