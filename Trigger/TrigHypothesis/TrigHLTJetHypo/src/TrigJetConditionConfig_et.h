/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGJETCONDITIONCONFIG_ET_H
#define TRIGJETCONDITIONCONFIG_ET_H

#include "ITrigJetConditionConfig.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "./ConditionsDefsMT.h"

class TrigJetConditionConfig_et:
public extends<AthAlgTool, ITrigJetConditionConfig> {

 public:
  
  TrigJetConditionConfig_et(const std::string& type,
                          const std::string& name,
                          const IInterface* parent);

  virtual StatusCode initialize() override;
  virtual ConditionMT getCondition() const override;

  virtual bool addToCapacity(std::size_t) override;
  virtual std::size_t capacity() const override;

 private:
  
  Gaudi::Property<std::string>
    m_min{this, "min", {}, "single jet min Et"};
  
  Gaudi::Property<std::string>
    m_max{this, "max", {}, "single jet max Et"};
  
  StatusCode checkVals()  const;
 
};
#endif
