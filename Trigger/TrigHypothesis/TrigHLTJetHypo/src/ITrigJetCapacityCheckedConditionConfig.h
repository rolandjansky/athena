/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGHLTJETHYPO_ITRIGJETCAPACITYCHECKEDCONDITIONCONFIG_H
#define TRIGHLTJETHYPO_ITRIGJETCAPACITYCHECKEDCONDITIONCONFIG_H

#include "GaudiKernel/IAlgTool.h"
#include "./CapacityCheckedConditionsDefs.h"

class ITrigJetCapacityCheckedConditionConfig : virtual public ::IAlgTool {
    
public:
  DeclareInterfaceID(ITrigJetCapacityCheckedConditionConfig, 1, 0);
  virtual ~ITrigJetCapacityCheckedConditionConfig(){};

  virtual ConditionPtr getCapacityCheckedCondition() const = 0;
  virtual ConditionPtr getCapacityCheckedAntiCondition() const = 0;

  virtual bool addToCapacity(std::size_t) = 0;
  virtual std::size_t capacity() const = 0;
};
#endif
