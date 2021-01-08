/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGJETCONDITIONCONFIG_HTFR_H
#define TRIGJETCONDITIONCONFIG_HTFR_H

/*
Condiguration AlgTool for ht conditions to be run in FastReduction
PS 
*/

#include "ITrigJetConditionConfig.h"
#include "./ConditionsDefsMT.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ConditionsDefs.h"

class TrigJetConditionConfig_htfr:
public extends<AthAlgTool, ITrigJetConditionConfig> {

 public:
  
  TrigJetConditionConfig_htfr(const std::string& type,
			      const std::string& name,
			      const IInterface* parent);

  virtual StatusCode initialize() override;
  virtual ConditionMT getCondition() const override;

  virtual bool addToCapacity(std::size_t) override;
  virtual std::size_t capacity() const override;

 private:


  Gaudi::Property<std::string>
    m_min{this, "min", {}, "min HT"};

  Gaudi::Property<std::string>
    m_max{this, "max", {}, "max HT"};

  StatusCode checkVals()  const;
};
#endif
