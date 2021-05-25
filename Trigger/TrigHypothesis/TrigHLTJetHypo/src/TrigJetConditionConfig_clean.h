/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGJETCONDITIONCONFIG_CLEAN_H
#define TRIGJETCONDITIONCONFIG_CLEAN_H

#include "ITrigJetConditionConfig.h"
#include "./ConditionsDefs.h"
#include "AthenaBaseComps/AthAlgTool.h"

class TrigJetConditionConfig_clean:
public extends<AthAlgTool, ITrigJetConditionConfig> {

 public:
  
  TrigJetConditionConfig_clean(const std::string& type, const std::string& name, const IInterface* parent);

  virtual StatusCode initialize() override;
  virtual Condition getCondition() const override;

 private:
  
  Gaudi::Property<std::string>
    m_cName{this, "val", {}, "Jet cleaning decoration name"};
  
  StatusCode checkVals()  const;
 
};
#endif
