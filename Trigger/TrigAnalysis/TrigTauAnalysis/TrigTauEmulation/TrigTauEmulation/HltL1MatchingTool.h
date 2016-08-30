/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HLTL1MATCHING_H
#define HLTL1MATCHING_H

#include "AsgTools/AsgTool.h"

#include "TrigTauEmulation/IHltL1MatchingTool.h"
#include "TrigTauEmulation/DecoratedHltTau.h"

class HltL1MatchingTool : public virtual IHltL1MatchingTool, public asg::AsgTool

{

  ASG_TOOL_CLASS(HltL1MatchingTool, IHltL1MatchingTool)

 public:
  
  HltL1MatchingTool(const std::string & name);
  HltL1MatchingTool(const HltL1MatchingTool & other);
  HltL1MatchingTool& operator=(const HltL1MatchingTool & other) = delete;

  virtual StatusCode initialize();

  virtual StatusCode execute(const xAOD::TauJetContainer * hlt_taus, const xAOD::EmTauRoIContainer * l1_taus);
  virtual StatusCode execute(const std::vector<DecoratedHltTau>& hlt_taus, const xAOD::EmTauRoIContainer * l1_taus);


 private:
  
  virtual StatusCode match(const xAOD::TauJet * hlt_tau, const xAOD::EmTauRoIContainer * l1_taus);
  double m_dr_cut;
  std::vector<std::string> m_l1seeds;

};
#endif
