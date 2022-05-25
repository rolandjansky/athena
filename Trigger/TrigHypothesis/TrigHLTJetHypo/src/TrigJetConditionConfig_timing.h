
/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGJETCONDITIONCONFIG_TIMING_H
#define TRIGJETCONDITIONCONFIG_TIMING_H


#include "ITrigJetConditionConfig.h"
#include "./ConditionsDefs.h"
#include "AthenaBaseComps/AthAlgTool.h"

class TrigJetConditionConfig_timing:
public extends<AthAlgTool, ITrigJetConditionConfig> {

 public:
  
  TrigJetConditionConfig_timing(const std::string& type, const std::string& name, const IInterface* parent);

  virtual StatusCode initialize() override;
  virtual Condition getCondition() const override;

 private:

  Gaudi::Property<std::string>
    m_min{this, "min", {}, "min timing value"};
  
  Gaudi::Property<std::string>
    m_max{this, "max", {}, "max timing value"};

  StatusCode checkVals()  const;
};
#endif
