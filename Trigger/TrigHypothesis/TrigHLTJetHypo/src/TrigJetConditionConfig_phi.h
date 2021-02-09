/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGJETCONDITIONCONFIG_PHI_H
#define TRIGJETCONDITIONCONFIG_PHI_H

#include "ITrigJetConditionConfig.h"
#include "./ConditionsDefsMT.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "./ConditionsDefsMT.h"
#include "./ArgStrToDouble.h"

class TrigJetConditionConfig_phi:
public extends<AthAlgTool, ITrigJetConditionConfig> {

 public:
  
  TrigJetConditionConfig_phi(const std::string& type,
			     const std::string& name,
			     const IInterface* parent);
  
  virtual StatusCode initialize() override;
  virtual ConditionMT getCondition() const override;

  virtual bool addToCapacity(std::size_t) override;
  virtual std::size_t capacity() const override;
  
 private:

  // phi min and max are strings. "PI" and "-PI" , as well
  // as any intermediate value that converts to a double value
  // in the range (-pi, pi), eg "1.0", are accepted.
  
  Gaudi::Property<std::string>
    m_strmin{this, "min", {}, "min for phi region"};
  
  Gaudi::Property<std::string>
    m_strmax{this, "max", {}, "max for phi region"};

  double m_min{0.};
  double m_max{0.};
  
  StatusCode checkVals()  const;
 
};
#endif
