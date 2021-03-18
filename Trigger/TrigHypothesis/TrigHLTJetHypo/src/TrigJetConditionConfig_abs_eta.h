/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGJETCONDITIONCONFIG_ABS_ETA_H
#define TRIGJETCONDITIONCONFIG_ABS_ETA_H

#include "ITrigJetConditionConfig.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "./ConditionsDefsMT.h"
#include "./ArgStrToDouble.h"

class TrigJetConditionConfig_abs_eta:
public extends<AthAlgTool, ITrigJetConditionConfig> {

 public:
  
  TrigJetConditionConfig_abs_eta(const std::string& type,
				 const std::string& name,
				 const IInterface* parent);

  virtual StatusCode initialize() override;
  virtual ConditionMT getCondition() const override;

  virtual bool addToCapacity(std::size_t) override;
  virtual std::size_t capacity() const override;
  
 private:
  
  Gaudi::Property<std::string>
    m_min{this, "min", {}, "Abs eta min for eta region"};
  
  Gaudi::Property<std::string>
    m_max{this, "max", {}, "Abs eta max for eta region"};

  StatusCode checkVals()  const;
 
};
#endif
