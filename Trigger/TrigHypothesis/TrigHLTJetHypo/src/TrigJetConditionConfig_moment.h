/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGJETCONDITIONCONFIG_MOMENT_H
#define TRIGJETCONDITIONCONFIG_MOMENT_H

#include "ITrigJetConditionConfig.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "./ConditionsDefs.h"

class TrigJetConditionConfig_moment:
public extends<AthAlgTool, ITrigJetConditionConfig> {

 public:
  
  TrigJetConditionConfig_moment(const std::string& type,
				const std::string& name,
				const IInterface* parent);

  virtual StatusCode initialize() override;
  virtual Condition getCondition() const override;

 private:
  
  Gaudi::Property<std::string>
    m_min{this, "min", {}, "jet moment min cut"};
  
  Gaudi::Property<std::string>
    m_max{this, "max", {}, "jet moment max cut"};

  Gaudi::Property<std::string>
    m_moment{this, "moment", {}, "jet moment name"};
  
  StatusCode checkVals()  const;
 
};
#endif
