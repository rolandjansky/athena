/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGHLTJETHYPO_ITRIGJETREPEATEDCONDITIONCONFIG_H
#define TRIGHLTJETHYPO_ITRIGJETREPEATEDCONDITIONCONFIG_H

#include "GaudiKernel/IAlgTool.h"
#include "./RepeatedConditionsDefs.h"

class ITrigJetRepeatedConditionConfig : virtual public ::IAlgTool {
    
public:
  DeclareInterfaceID(ITrigJetRepeatedConditionConfig, 1, 0);
  virtual ~ITrigJetRepeatedConditionConfig(){};

  virtual ConditionPtr getRepeatedCondition() const = 0;

};
#endif
