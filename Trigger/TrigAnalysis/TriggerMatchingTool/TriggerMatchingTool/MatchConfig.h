// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MATCHCONFIG_H
#define MATCHCONFIG_H

#include "TriggerMatchingTool/IMatchConfig.h"

#include <map>

namespace Trig {
  
  class MatchConfig : public IMatchConfig {
  public:
    MatchConfig();
    MatchConfig(const std::map<xAOD::Type::ObjectType, double>& thresholds);
    ~MatchConfig();
    virtual double matchThreshold(xAOD::Type::ObjectType) const override;
  private:
    //we'll keep a type_sorted list to the reco particles that initiated this match
    std::map<xAOD::Type::ObjectType,double> m_thresholds_by_type;
  };
  
}

#endif
