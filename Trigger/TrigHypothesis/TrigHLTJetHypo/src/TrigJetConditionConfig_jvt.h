/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGJETCONDITIONCONFIG_JVT_H
#define TRIGJETCONDITIONCONFIG_JVT_H


#include "ITrigJetConditionConfig.h"
#include "./ConditionsDefsMT.h"
#include "AthenaBaseComps/AthAlgTool.h"

class TrigJetConditionConfig_jvt:
public extends<AthAlgTool, ITrigJetConditionConfig> {

 public:
  
  TrigJetConditionConfig_jvt(const std::string& type, const std::string& name, const IInterface* parent);

  virtual StatusCode initialize() override;
  virtual ConditionMT getCondition() const override;

  virtual bool addToCapacity(std::size_t) override;
  virtual std::size_t capacity() const override;

 private:

  Gaudi::Property<std::string>
    m_min{this, "min", {}, "min jvt value"};
  
  Gaudi::Property<std::string>
    m_max{this, "max", {}, "max jvt value"};

  StatusCode checkVals()  const;
};
#endif
