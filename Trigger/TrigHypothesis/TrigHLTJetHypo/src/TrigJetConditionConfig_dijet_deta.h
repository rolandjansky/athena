/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGJETCONDITIONCONFIG_DIJET_DETA_H
#define TRIGJETCONDITIONCONFIG_DIJET_DETA_H


#include "ITrigJetConditionConfig.h"
#include "./ConditionsDefsMT.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ConditionsDefs.h"

class TrigJetConditionConfig_dijet_deta:
public extends<AthAlgTool, ITrigJetConditionConfig> {

 public:
  
  TrigJetConditionConfig_dijet_deta(const std::string& type,
				    const std::string& name,
				    const IInterface* parent);

  virtual StatusCode initialize() override;
  virtual ConditionMT getCondition() const override;


 private:


  Gaudi::Property<std::string>
    m_min{this, "deta_min", {}, "min dijet dEta"};

  Gaudi::Property<std::string>
    m_max{this, "deta_max", {}, "max dijet dEta"};

  StatusCode checkVals()  const;
};
#endif
