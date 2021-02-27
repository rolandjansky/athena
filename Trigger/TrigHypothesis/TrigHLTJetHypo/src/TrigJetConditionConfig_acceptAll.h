/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGJETCONDITIONCONFIG_ACCEPTALL_H
#define TRIGJETCONDITIONCONFIG_ACCEPTALL_H

#include "ITrigJetConditionConfig.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "./ConditionsDefsMT.h"

class TrigJetConditionConfig_acceptAll:
public extends<AthAlgTool, ITrigJetConditionConfig> {

 public:
  
  TrigJetConditionConfig_acceptAll(const std::string& type,
                          const std::string& name,
                          const IInterface* parent);

  virtual ConditionMT getCondition() const override;

  virtual bool addToCapacity(std::size_t) override;
  virtual std::size_t capacity() const override;
 private:

  std::size_t m_capacity{0};  // not a property: set at run time

};
#endif
