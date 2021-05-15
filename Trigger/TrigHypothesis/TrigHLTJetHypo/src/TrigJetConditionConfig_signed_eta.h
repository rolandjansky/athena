/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGJETCONDITIONCONFIG_SIGNED_ETA_H
#define TRIGJETCONDITIONCONFIG_SIGNED_ETA_H

#include "ITrigJetConditionConfig.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "./ConditionsDefs.h"
#include "./ArgStrToDouble.h"

class TrigJetConditionConfig_signed_eta:
public extends<AthAlgTool, ITrigJetConditionConfig> {

 public:
  
  TrigJetConditionConfig_signed_eta(const std::string& type,
				    const std::string& name,
				    const IInterface* parent);

  virtual StatusCode initialize() override;
  virtual Condition getCondition() const override;

 private:
  
  Gaudi::Property<std::string>
    m_min{this, "min", {"-inf"}, "Abs eta min for eta region"};
  
  Gaudi::Property<std::string>
    m_max{this, "max", {"inf"}, "Abs eta max for eta region"};

  StatusCode checkVals()  const;
 
};
#endif
