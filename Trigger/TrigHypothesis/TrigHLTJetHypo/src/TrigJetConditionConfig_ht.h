/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGJETCONDITIONCONFIG_HT_H
#define TRIGJETCONDITIONCONFIG_HT_H

#include "ITrigJetConditionConfig.h"
#include "./ConditionsDefsMT.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ConditionsDefs.h"

class TrigJetConditionConfig_ht:
public extends<AthAlgTool, ITrigJetConditionConfig> {

 public:
  
  TrigJetConditionConfig_ht(const std::string& type,
                          const std::string& name,
                          const IInterface* parent);

  virtual StatusCode initialize() override;
  virtual ConditionMT getCondition() const override;

  virtual bool addToCapacity(std::size_t) override;
  virtual std::size_t capacity() const override;

 private:

  // cut on sum
  Gaudi::Property<std::string>
    m_htmin{this, "htmin", {"0"}, "min ht (et sum for selected jets)"};

  // cuts on input vectors
  
  Gaudi::Property<std::string>
    m_etmin{this, "etmin", {"0"}, "min et for each jet"};
  
  Gaudi::Property<std::string>
    m_etamin{this, "etamin", {"0"}, "min eta for each jet"};
  
  Gaudi::Property<std::string>
    m_etamax{this, "etamax", {"inf"}, "max eta for each jet"};
  
  StatusCode checkVals()  const;
};
#endif
