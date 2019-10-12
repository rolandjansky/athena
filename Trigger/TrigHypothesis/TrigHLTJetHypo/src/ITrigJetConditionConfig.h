/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGHLTJETHYPO_ITRIGJETCONDITIONCONFIG_H
#define TRIGHLTJETHYPO_ITRIGJETCONDITIONCONFIG_H

#include "GaudiKernel/IAlgTool.h"
#include "./ConditionsDefsMT.h"

class ITrigJetConditionConfig : virtual public ::IAlgTool {
    
public:
  DeclareInterfaceID(ITrigJetConditionConfig, 1, 0);
  virtual ~ITrigJetConditionConfig(){};

  virtual ConditionMT getCondition() const = 0;
  virtual bool addToCapacity(std::size_t) = 0;
  virtual std::size_t capacity() const = 0;
};
#endif
