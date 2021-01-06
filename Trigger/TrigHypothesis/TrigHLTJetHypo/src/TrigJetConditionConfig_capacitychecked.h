/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGJETCONDITIONCONFIG_CAPACITYCHECKED_H
#define TRIGJETCONDITIONCONFIG_CAPACITYCHECKED_H

#include "CapacityCheckedConditionsDefs.h"

#include "ITrigJetCapacityCheckedConditionConfig.h"
#include "ITrigJetConditionConfig.h"
#include "AthenaBaseComps/AthAlgTool.h"

class TrigJetConditionConfig_capacitychecked:
public extends<AthAlgTool, ITrigJetCapacityCheckedConditionConfig> {

 public:
  
  TrigJetConditionConfig_capacitychecked(const std::string& type,
				  const std::string& name,
				  const IInterface* parent);

  virtual StatusCode initialize() override;
  virtual ConditionPtr getCapacityCheckedCondition() const override;

  virtual bool addToCapacity(std::size_t) override;
  virtual std::size_t capacity() const override;

 private:

  ToolHandleArray<ITrigJetConditionConfig> m_elementConditions{
    this, "conditionMakers", {},
      "elemental conditions makers for a leaf node."};

  Gaudi::Property<std::size_t> m_multiplicity {this, "multiplicity", {1},
      "no. of occurences of identical condition"};

  

  Gaudi::Property<int> m_chainPartInd {this,
    "chainPartInd",
    {-1},
    "identifier for chain leg - used to group jets for jet hypo clients "};



  StatusCode checkVals()  const;
 
};
#endif
