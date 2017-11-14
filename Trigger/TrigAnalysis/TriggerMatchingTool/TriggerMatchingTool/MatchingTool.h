// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGERMATCHINGTOOL_MATCHINGTOOL_H
#define TRIGGERMATCHINGTOOL_MATCHINGTOOL_H

// Framework include(s):
#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"

#include "TriggerMatchingTool/IMatchingTool.h"
#include "TriggerMatchingTool/TypeMap.h"
#include "TriggerMatchingTool/IMatchingTool.h"
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
  StatusCode initialize();

  bool match(const std::vector<const xAOD::IParticle*>& recoObjects, const std::string& chain, bool rerun);
  bool match(const std::vector<const xAOD::IParticle*>& recoObjects, const std::string& chain, bool rerun, double matchTreshold);
  bool match(const xAOD::IParticle& recoObjects, const std::string& chain, bool rerun, double matchTreshold);

protected:
  MatchingImplementation* impl();

  bool matchSingleType(const std::vector<const xAOD::IParticle*>& subRecoObjects, Trig::Combination& comb);

  bool matchCombination(const std::vector<const xAOD::IParticle*>& recoObjects, Trig::Combination& comb);


#ifndef XAOD_STANDALONE
  void updateOutputLevel(Property& p);
  void updateThreshold(Property& p);
#endif

private:

  double IParticleMetric(const xAOD::IParticle* lhs, const xAOD::IParticle* rhs);
  std::vector<std::vector<double> > distanceMatrix(const std::vector<const xAOD::IParticle*>& reco,
						   const std::vector<const xAOD::IParticle*>& trigger);
  
  
  MatchingImplementation* m_impl;  
  TypeMap m_typeMap;
  ToolHandle<Trig::TrigDecisionTool> m_trigDecTool;
  double m_matchingThreshold;
};

}

#endif
