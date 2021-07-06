/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGJETCONDITIONCONFIG_PT_H
#define TRIGJETCONDITIONCONFIG_PT_H

#include "ITrigJetConditionConfig.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "./ConditionsDefs.h"

class TrigJetConditionConfig_pt:
public extends<AthAlgTool, ITrigJetConditionConfig> {

 public:
  
  TrigJetConditionConfig_pt(const std::string& type,
                          const std::string& name,
                          const IInterface* parent);

  virtual StatusCode initialize() override;
  virtual Condition getCondition() const override;

 private:
  
  Gaudi::Property<std::string>
    m_min{this, "min", {}, "single jet min Pt"};
  
  Gaudi::Property<std::string>
    m_max{this, "max", {}, "single jet max Pt"};
  
  StatusCode checkVals()  const;
 
};
#endif
