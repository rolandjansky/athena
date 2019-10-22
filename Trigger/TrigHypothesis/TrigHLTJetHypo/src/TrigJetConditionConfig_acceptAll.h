/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGJETCONDITIONCONFIG_ACCEPTALL_H
#define TRIGJETCONDITIONCONFIG_ACCEPTALL_H

#include "ITrigJetConditionConfig.h"
#include "./ConditionsDefsMT.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "./ConditionsDefsMT.h"

class TrigJetConditionConfig_acceptAll:
public extends<AthAlgTool, ITrigJetConditionConfig> {

 public:
  
  TrigJetConditionConfig_acceptAll(const std::string& type,
                          const std::string& name,
                          const IInterface* parent);

  virtual ConditionMT getCondition() const override;

 private:
  Gaudi::Property<std::size_t>
    m_capacity {this, "capacity", {}, "Conditon capacity"};
 
};
#endif
