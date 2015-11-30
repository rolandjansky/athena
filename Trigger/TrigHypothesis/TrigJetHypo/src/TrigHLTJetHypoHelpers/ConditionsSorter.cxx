/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigJetHypo/TrigHLTJetHypoUtils/ConditionsSorter.h"
#include "TrigJetHypo/TrigHLTJetHypoUtils/ConditionBridge.h"

bool ConditionsSorter::operator()(const ConditionBridge& l,
				  const ConditionBridge& r){
  // return l.threshold() > r.threshold();
  return l.orderingParameter() > r.orderingParameter();
}
