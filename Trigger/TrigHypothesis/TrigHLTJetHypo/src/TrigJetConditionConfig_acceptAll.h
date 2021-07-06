/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGJETCONDITIONCONFIG_ACCEPTALL_H
#define TRIGJETCONDITIONCONFIG_ACCEPTALL_H

#include "ITrigJetConditionConfig.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "./ConditionsDefs.h"

class TrigJetConditionConfig_acceptAll:
public extends<AthAlgTool, ITrigJetConditionConfig> {

 public:
  
  TrigJetConditionConfig_acceptAll(const std::string& type,
                          const std::string& name,
                          const IInterface* parent);

  virtual Condition getCondition() const override;

};
#endif
