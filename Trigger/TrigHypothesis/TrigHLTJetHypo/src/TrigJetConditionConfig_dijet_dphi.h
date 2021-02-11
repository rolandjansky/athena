/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGJETCONDITIONCONFIG_DIJET_DPHI_H
#define TRIGJETCONDITIONCONFIG_DIJET_DPHI_H


#include "ITrigJetConditionConfig.h"
#include "./ConditionsDefsMT.h"
#include "AthenaBaseComps/AthAlgTool.h"

class TrigJetConditionConfig_dijet_dphi:
public extends<AthAlgTool, ITrigJetConditionConfig> {
  
 public:
  
  TrigJetConditionConfig_dijet_dphi(const std::string& type,
				    const std::string& name,
				    const IInterface* parent);
  
  virtual StatusCode initialize() override;
  virtual ConditionMT getCondition() const override;

  virtual bool addToCapacity(std::size_t) override;
  virtual std::size_t capacity() const override;
  
  
 private:
  
  Gaudi::Property<std::string>
    m_min{this, "min", {}, "min dijet dPhi"};
  
  Gaudi::Property<std::string>
    m_max{this, "max", {}, "max dojet dPhi"};

  StatusCode checkVals()  const;
};
#endif
