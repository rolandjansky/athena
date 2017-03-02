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
#include "TrigDecisionTool/Combination.h"
#include "TrigDecisionTool/TrigDecisionTool.h"

#include "TriggerMatchingTool/MatchResult.h"





namespace Trig {

  typedef std::map<xAOD::Type::ObjectType,std::vector<const xAOD::IParticle*> >  IParticlesByType;
  

  
class MatchingTool : public asg::AsgTool, 
		     virtual public Trig::IMatchingTool {
  ASG_TOOL_CLASS(MatchingTool,IMatchingTool)

public:
  friend class MatchingImplementation;
  MatchingTool(const std::string& name);
  ~MatchingTool();
  StatusCode initialize() override;
  
  //convenience method for multi-object method with single threshold
  bool match(const std::vector<const xAOD::IParticle*>& recoObjects, const std::string& chain, double matchTreshold) override;
  
  //convenience method for multi-object method with type-specific threshold
  bool match(const std::vector<const xAOD::IParticle*>& recoObjects, const std::string& chain, const std::map<xAOD::Type::ObjectType, double>& thresholds) override;
  
  //convenience method for single-object method
  bool match(const xAOD::IParticle& recoObjects, const std::string& chain, double matchTreshold) override;
  
  bool match(const std::vector<const xAOD::IParticle*>& recoObjects, const std::string& chain);

  //main matching method
  std::unique_ptr<IMatchResult> match_result(const std::vector<const xAOD::IParticle*>& recoObjects, const std::string& chain, const IMatchConfig* mc = 0) override;
  
protected:
  MatchingImplementation* impl() override;

  bool matchSingleType(const std::vector<const xAOD::IParticle*>& subRecoObjects, Trig::Combination& comb, ComboMatch::SingleTypeMatch& result, double threshold);

  bool matchCombination(IParticlesByType& recoByType, Trig::Combination& comb, std::unique_ptr<MatchResult>& result, const IMatchConfig* mc = 0);


#ifndef XAOD_STANDALONE
  void updateOutputLevel(Property& p);
  void updateThreshold(Property& p);
#endif

private:


  IParticlesByType separateByType(const std::vector<const xAOD::IParticle*>& recoObjects);

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
