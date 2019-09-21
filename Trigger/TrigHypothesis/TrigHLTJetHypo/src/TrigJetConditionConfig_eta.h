/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGJETCONDITIONCONFIG_ETA_H
#define TRIGJETCONDITIONCONFIG_ETA_H

#include "ITrigJetConditionConfig.h"
#include "./ConditionsDefsMT.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "./ConditionsDefsMT.h"
#include "./ArgStrToDouble.h"

class TrigJetConditionConfig_eta:
public extends<AthAlgTool, ITrigJetConditionConfig> {

 public:
  
  TrigJetConditionConfig_eta(const std::string& type,
                          const std::string& name,
                          const IInterface* parent);

  virtual StatusCode initialize() override;
  virtual ConditionMT getCondition() const override;

 private:
  
  Gaudi::Property<std::string>
    m_min{this, "eta_min", {}, "Abs eta min for eta region"};
  
  Gaudi::Property<std::string>
    m_max{this, "eta_max", {}, "Abs eta max for eta region"};

  StatusCode checkVals()  const;
 
};
#endif
