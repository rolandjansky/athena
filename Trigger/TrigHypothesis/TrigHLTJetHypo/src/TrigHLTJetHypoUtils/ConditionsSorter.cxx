/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ConditionsSorter.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ConditionBridge.h"

bool ConditionsSorter::operator()(const ConditionBridge& l,
				  const ConditionBridge& r){
  // return l.threshold() > r.threshold();
  return l.orderingParameter() > r.orderingParameter();
}
