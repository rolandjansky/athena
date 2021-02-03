/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGJETCONDITIONCONFIG_REPEATED_H
#define TRIGJETCONDITIONCONFIG_REPEATED_H

#include "RepeatedConditionsDefs.h"
#include "./IConditionMT.h"

#include "ITrigJetRepeatedConditionConfig.h"
#include "ITrigJetConditionConfig.h"
#include "AthenaBaseComps/AthAlgTool.h"

class TrigJetConditionConfig_repeated:
public extends<AthAlgTool, ITrigJetRepeatedConditionConfig> {

 public:
  
  TrigJetConditionConfig_repeated(const std::string& type,
				  const std::string& name,
				  const IInterface* parent);

  virtual StatusCode initialize() override;
  virtual ConditionPtr getRepeatedCondition() const override;
  virtual ConditionPtr getRepeatedAntiCondition() const override;


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


  // make a Compound Condition, used by, eg, RepeatedCondition
  // or ConditionInverter 
  std::unique_ptr<IConditionMT> getCompoundCondition() const;
  StatusCode checkVals()  const;
 
};
#endif
