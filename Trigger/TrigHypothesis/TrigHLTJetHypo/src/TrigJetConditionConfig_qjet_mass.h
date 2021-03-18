/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGJETCONDITIONCONFIG_QJET_MASS_H
#define TRIGJETCONDITIONCONFIG_QJET_MASS_H


#include "ITrigJetConditionConfig.h"
#include "./ConditionsDefsMT.h"
#include "AthenaBaseComps/AthAlgTool.h"

class TrigJetConditionConfig_qjet_mass:
public extends<AthAlgTool, ITrigJetConditionConfig> {

 public:
  
  TrigJetConditionConfig_qjet_mass(const std::string& type,
                          const std::string& name,
                          const IInterface* parent);

  virtual StatusCode initialize() override;
  virtual ConditionMT getCondition() const override;

  virtual bool addToCapacity(std::size_t) override;
  virtual std::size_t capacity() const override;
  
 private:

  Gaudi::Property<std::string>
    m_min{this, "min", {}, "min mass for each qjet_mass"};

  Gaudi::Property<std::string>
    m_max{this, "max", {}, "max mass for each qjet_mass"};

  StatusCode checkVals()  const;
};
#endif
