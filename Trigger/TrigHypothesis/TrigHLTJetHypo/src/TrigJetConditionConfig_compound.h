/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGJETCONDITIONCONFIG_COMPOUND_H
#define TRIGJETCONDITIONCONFIG_COMPOUND_H


#include "ITrigJetConditionConfig.h"
#include "AthenaBaseComps/AthAlgTool.h"

class TrigJetConditionConfig_compound:
public extends<AthAlgTool, ITrigJetConditionConfig> {

 public:
  
  TrigJetConditionConfig_compound(const std::string& type,
				  const std::string& name,
				  const IInterface* parent);

  virtual StatusCode initialize() override;
  virtual ConditionMT getCondition() const override;

 private:

  ToolHandleArray<ITrigJetConditionConfig> m_elementConditions{
    this, "conditionMakers", {},
      "elemental conditions makers for a leaf node."};
  
  StatusCode checkVals()  const;
 
};
#endif
