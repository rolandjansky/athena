/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_CAPCACITYCHECKEDCONDITIONSDEFSMT_H
#define TRIGHLTJETHYPO_CAPCACITYCHECKEDCONDITIONSDEFSMT_H
/********************************************************************
 *
 * NAME:     ConditionsDefs.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 * AUTHOR:   P. Sherwood
 *********************************************************************/

#include <string>
#include <memory>
#include "./ICapacityCheckedCondition.h"

using ConditionPtr = std::unique_ptr<ICapacityCheckedCondition>;
using ConditionPtrs = std::vector<ConditionPtr>;
using ConditionPtrsIter = ConditionPtrs::iterator;

#endif
