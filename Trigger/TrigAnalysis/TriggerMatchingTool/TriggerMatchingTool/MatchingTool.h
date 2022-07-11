// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGERMATCHINGTOOL_MATCHINGTOOL_H
#define TRIGGERMATCHINGTOOL_MATCHINGTOOL_H

// Framework include(s):
#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"

#include "TriggerMatchingTool/IMatchingTool.h"
#include "TriggerMatchingTool/IMatchScoringTool.h"
#include "TriggerMatchingTool/TypeMap.h"
#include "TrigDecisionTool/Combination.h"
#include "TrigDecisionTool/TrigDecisionTool.h"

namespace Trig {

class MatchingTool : public asg::AsgTool, 
		     virtual public Trig::IMatchingTool {
  ASG_TOOL_CLASS(MatchingTool,IMatchingTool)

public:
  friend class MatchingImplementation;
  MatchingTool(const std::string& name);
  ~MatchingTool();
  StatusCode initialize() override;

  bool match(const std::vector<const xAOD::IParticle*>& recoObjects, const std::string& chain, double matchTreshold, bool rerun) const override;
  bool match(const xAOD::IParticle& recoObjects, const std::string& chain, double matchTreshold, bool rerun) const override;

protected:
  const MatchingImplementation* impl() const override;

  bool matchSingleType(const std::vector<const xAOD::IParticle*>& subRecoObjects, Trig::Combination& comb, const std::string& chain, double threshold) const;

  bool matchCombination(const std::vector<const xAOD::IParticle*>& recoObjects, Trig::Combination& comb, const std::string& chain, double threshold) const;


#ifndef XAOD_STANDALONE
  void updateThreshold(Gaudi::Details::PropertyBase& p);
#endif

private:
  
  MatchingImplementation* m_impl;  
  TypeMap m_typeMap;
  ToolHandle<Trig::TrigDecisionTool> m_trigDecTool;
  ToolHandle<Trig::IMatchScoringTool> m_scoreTool{
      this, "ScoringTool", "Trig::DRScoringTool","Tool to score pairs of particles"};
  double m_matchingThreshold;
};

}

#endif
