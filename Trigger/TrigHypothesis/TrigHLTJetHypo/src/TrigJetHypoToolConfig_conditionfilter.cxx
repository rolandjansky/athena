/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigJetHypoToolConfig_conditionfilter.h"
#include "GaudiKernel/StatusCode.h"
#include "./RepeatedCondition.h"

TrigJetHypoToolConfig_conditionfilter::TrigJetHypoToolConfig_conditionfilter(const std::string& type,
                                                 const std::string& name,
                                                 const IInterface* parent) :
  base_class(type, name, parent){
}


TrigJetHypoToolConfig_conditionfilter::~TrigJetHypoToolConfig_conditionfilter(){
}

StatusCode TrigJetHypoToolConfig_conditionfilter::initialize() {
  ATH_MSG_INFO("initialising " << name());
  return StatusCode::SUCCESS;
}


ConditionPtrs
TrigJetHypoToolConfig_conditionfilter::getRepeatedConditions() const {

  ConditionPtrs conditions;

  // collect the Conditions objects from the various sources

  for(const auto& cm : m_conditionMakers){
    conditions.push_back(cm->getRepeatedCondition());
  }
      
  return conditions;
}


FilterPtr
TrigJetHypoToolConfig_conditionfilter::getHypoJetVectorFilter() const {

  /* set up the prefilter by looping over the precondition 
     Condition maker AlgTools to obtain the elemental Conditions,
     place these in a single compound Condition, and warp this in a
     CondtionInverter. This is passed to the ConditionFilter object.
  */

  // if no conditions the filter will apply n inverter to an empty
  // Compound Condition, which will kill all events.
  if (m_conditionMakers.empty()) {
    return std::unique_ptr<ConditionFilter>(nullptr);
  }
  
  auto makeElementalFilterCondition = [](auto& conditionMaker)->ConditionPtr {
    return conditionMaker->getRepeatedCondition();
  };

  // fill a container with pointers to an elemental condition
  // note: IRepeatedCondition derives from ICondition
  ConditionPtrs prefilterConditions{};
  std::transform(m_conditionMakers.begin(),
		 m_conditionMakers.end(),
		 std::back_inserter(prefilterConditions),
		 makeElementalFilterCondition);

  // create an filter from the vector containing the inverted condition.
  return std::make_unique<ConditionFilter>(std::move(prefilterConditions));
}

