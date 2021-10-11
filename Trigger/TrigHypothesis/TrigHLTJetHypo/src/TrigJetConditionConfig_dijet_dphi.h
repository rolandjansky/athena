/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGJETCONDITIONCONFIG_DIJET_DPHI_H
#define TRIGJETCONDITIONCONFIG_DIJET_DPHI_H


#include "ITrigJetConditionConfig.h"
#include "./ConditionsDefs.h"
#include "AthenaBaseComps/AthAlgTool.h"

class TrigJetConditionConfig_dijet_dphi:
public extends<AthAlgTool, ITrigJetConditionConfig> {
  
 public:
  
  TrigJetConditionConfig_dijet_dphi(const std::string& type,
				    const std::string& name,
				    const IInterface* parent);
  
  virtual StatusCode initialize() override;
  virtual Condition getCondition() const override;

 private:
  
  Gaudi::Property<std::string>
    m_min{this, "min", {}, "min dijet dPhi"};
  
  Gaudi::Property<std::string>
    m_max{this, "max", {}, "max dojet dPhi"};

  StatusCode checkVals()  const;
};
#endif
