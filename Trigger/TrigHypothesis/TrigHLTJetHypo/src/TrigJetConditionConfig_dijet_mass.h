/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGJETCONDITIONCONFIG_DIJET_MASS_H
#define TRIGJETCONDITIONCONFIG_DIJET_MASS_H


#include "ITrigJetConditionConfig.h"
#include "./ConditionsDefs.h"
#include "AthenaBaseComps/AthAlgTool.h"

class TrigJetConditionConfig_dijet_mass:
public extends<AthAlgTool, ITrigJetConditionConfig> {

 public:
  
  TrigJetConditionConfig_dijet_mass(const std::string& type,
                          const std::string& name,
                          const IInterface* parent);

  virtual StatusCode initialize() override;
  virtual Condition getCondition() const override;

 private:

  Gaudi::Property<std::string>
    m_min{this, "min", {}, "min mass for each dijet_mass"};

  Gaudi::Property<std::string>
    m_max{this, "max", {}, "max mass for each dijet_mass"};

  StatusCode checkVals()  const;
};
#endif
