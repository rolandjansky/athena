/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGJETCONDITIONCONFIG_SMC_H
#define TRIGJETCONDITIONCONFIG_SMC_H

#include "ITrigJetConditionConfig.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "./ConditionsDefsMT.h"
#include "./ArgStrToDouble.h"

class TrigJetConditionConfig_smc:
public extends<AthAlgTool, ITrigJetConditionConfig> {

 public:
  
  TrigJetConditionConfig_smc(const std::string& type,
			     const std::string& name,
			     const IInterface* parent);

  virtual StatusCode initialize() override;
  virtual ConditionMT getCondition() const override;

  virtual bool addToCapacity(std::size_t) override;
  virtual std::size_t capacity() const override;
 private:
  
  Gaudi::Property<std::string>
    m_min{this, "min", {}, "min jet mass"};
  
  Gaudi::Property<std::string>
    m_max{this, "max", {}, "max jet mass"};

  StatusCode checkVals()  const;
 
};
#endif
