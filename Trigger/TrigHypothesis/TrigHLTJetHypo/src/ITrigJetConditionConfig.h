/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGHLTJETHYPO_ITRIGJETCONDITIONCONFIG_H
#define TRIGHLTJETHYPO_ITRIGJETCONDITIONCONFIG_H

#include "GaudiKernel/IAlgTool.h"
#include "./ConditionsDefsMT.h"
#include <optional>

class ITrigJetConditionConfig : virtual public ::IAlgTool {
    
public:
  DeclareInterfaceID(ITrigJetConditionConfig, 1, 0);
  virtual ~ITrigJetConditionConfig(){};

  virtual std::optional<ConditionsMT> getConditions() const = 0;
};
#endif
