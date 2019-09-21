/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGJETCONDITIONCONFIG_DIJET_MASS_H
#define TRIGJETCONDITIONCONFIG_DIJET_MASS_H


#include "ITrigJetConditionConfig.h"
#include "./ConditionsDefsMT.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ConditionsDefs.h"

class TrigJetConditionConfig_dijet_mass:
public extends<AthAlgTool, ITrigJetConditionConfig> {

 public:
  
  TrigJetConditionConfig_dijet_mass(const std::string& type,
                          const std::string& name,
                          const IInterface* parent);

  virtual StatusCode initialize() override;
  virtual ConditionMT getCondition() const override;


 private:

  Gaudi::Property<std::string>
    m_min{this, "mass_min", {}, "min mass for each dijet_mass"};

  Gaudi::Property<std::string>
    m_max{this, "mass_max", {}, "max mass for each dijet_mass"};

  StatusCode checkVals()  const;
};
#endif
