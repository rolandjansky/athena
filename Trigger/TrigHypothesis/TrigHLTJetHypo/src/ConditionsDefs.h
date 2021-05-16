/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_CONDITIONSDEFS_H
#define TRIGHLTJETHYPO_CONDITIONSDEFS_H
/********************************************************************
 *
 * NAME:     ConditionsDefs.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 * AUTHOR:   P. Sherwood
 *********************************************************************/

#include <string>
#include <memory>
#include "./ICondition.h"

using Condition = std::unique_ptr<ICondition>;
using Conditions = std::vector<Condition>;
using ConditionsIter = Conditions::iterator;

#endif
