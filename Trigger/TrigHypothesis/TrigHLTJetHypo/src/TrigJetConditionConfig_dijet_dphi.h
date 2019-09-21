/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGJETCONDITIONCONFIG_DIJET_DPHI_H
#define TRIGJETCONDITIONCONFIG_DIJET_DPHI_H


#include "ITrigJetConditionConfig.h"
#include "./ConditionsDefsMT.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ConditionsDefs.h"

class TrigJetConditionConfig_dijet_dphi:
public extends<AthAlgTool, ITrigJetConditionConfig> {
  
 public:
  
  TrigJetConditionConfig_dijet_dphi(const std::string& type,
				    const std::string& name,
				    const IInterface* parent);
  
  virtual StatusCode initialize() override;
  virtual ConditionMT getCondition() const override;
  
  
 private:
  
  Gaudi::Property<std::string>
    m_min{this, "dphi_min", {}, "min dijet dPhi"};
  
  Gaudi::Property<std::string>
    m_max{this, "dphi_max", {}, "max dojet dPhi"};

  StatusCode checkVals()  const;
};
#endif
