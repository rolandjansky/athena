/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGJETCONDITIONCONFIG_ETAET_H
#define TRIGJETCONDITIONCONFIG_ETAET_H

#include "ITrigJetConditionConfig.h"
#include "./ConditionsDefsMT.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "./ConditionsDefsMT.h"

class TrigJetConditionConfig_etaet:
public extends<AthAlgTool, ITrigJetConditionConfig> {

 public:
  
  TrigJetConditionConfig_etaet(const std::string& type,
                          const std::string& name,
                          const IInterface* parent);
  virtual ~TrigJetConditionConfig_etaet();

  virtual StatusCode initialize() override;
  virtual std::optional<ConditionsMT> getConditions() const override;

 private:
  
  Gaudi::Property<std::vector<std::string>>
    m_EtThresholds{this, "EtThresholds", {}, "Etthresholds by eta region"};
  
  Gaudi::Property<std::vector<std::string>>
    m_etaMins{this, "eta_mins", {}, "Eta min for eta regions"};
  
  Gaudi::Property<std::vector<std::string>>
    m_etaMaxs{this, "eta_maxs", {}, "Eta max for eta regions"};

  Gaudi::Property<std::vector<int>>
    m_asymmetricEtas{this, "asymmetricEtas", {}, "Apply asym. eta cuts"};


  virtual StatusCode checkVals()  const override;
 
};
#endif
