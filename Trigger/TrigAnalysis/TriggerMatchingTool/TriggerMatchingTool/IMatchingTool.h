// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IMATCHINGTOOL_H
#define IMATCHINGTOOL_H

#include <map>

// Framework include(s):
#include "AsgTools/IAsgTool.h"

#include "TriggerMatchingTool/IMatchResult.h"
#include "TriggerMatchingTool/IMatchConfig.h"

namespace xAOD{
  class IParticle;
}



namespace Trig {

class MatchingImplementation;

class IMatchingTool : virtual public asg::IAsgTool {
  ASG_TOOL_INTERFACE(IMatchingTool)
public:

  ///single object trigger matching. matchThreshold is typically the deltaR requirement to obtain positive matching
  virtual bool match(const xAOD::IParticle& recoObject, const std::string& chain, double matchThreshold=0.1) = 0;
  
  ///multi-object trigger matching with single threshold
  virtual bool match(const std::vector<const xAOD::IParticle*>& recoObjects, const std::string& chain, double matchThreshold=0.1) = 0;
  
  ///multi-object trigger matching
  virtual bool match(const std::vector<const xAOD::IParticle*>& recoObjects, const std::string& chain, const std::map<xAOD::Type::ObjectType, double>& thresholds) = 0;
  
  ///multi-object trigger matching returning a match result object
  virtual std::unique_ptr<IMatchResult> match_result(const std::vector<const xAOD::IParticle*>& recoObjects, const std::string& chain, const IMatchConfig* mc = nullptr) = 0;

protected:
  virtual MatchingImplementation* impl() = 0;
};

}

#endif
