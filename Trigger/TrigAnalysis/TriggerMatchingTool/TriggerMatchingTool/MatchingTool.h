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
  StatusCode initialize() override;

  bool match(const std::vector<const xAOD::IParticle*>& recoObjects, const std::string& chain, double matchTreshold, bool rerun) const override;
  bool match(const xAOD::IParticle& recoObjects, const std::string& chain, double matchTreshold, bool rerun) const override;

protected:
  MatchingImplementation* impl() const override;

  bool matchSingleType(const std::vector<const xAOD::IParticle*>& subRecoObjects, Trig::Combination& comb, const std::string& chain, double threshold) const;

  bool matchCombination(const std::vector<const xAOD::IParticle*>& recoObjects, Trig::Combination& comb, const std::string& chain, double threshold) const;


#ifndef XAOD_STANDALONE
  void updateThreshold(Gaudi::Details::PropertyBase& p);
#endif

private:

  double IParticleMetric(const xAOD::IParticle* lhs, const xAOD::IParticle* rhs) const ;
  std::vector<std::vector<double> > distanceMatrix(const std::vector<const xAOD::IParticle*>& reco,
						   const std::vector<const xAOD::IParticle*>& trigger)const;
  
  
  MatchingImplementation* m_impl;  
  TypeMap m_typeMap;
  ToolHandle<Trig::TrigDecisionTool> m_trigDecTool;
  double m_matchingThreshold;
};

}

#endif
