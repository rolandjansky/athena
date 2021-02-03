/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
#include "./IRepeatedCondition.h"

using ConditionPtr = std::unique_ptr<IRepeatedCondition>;
using ConditionPtrs = std::vector<ConditionPtr>;
using ConditionPtrsIter = ConditionPtrs::iterator;

#endif
