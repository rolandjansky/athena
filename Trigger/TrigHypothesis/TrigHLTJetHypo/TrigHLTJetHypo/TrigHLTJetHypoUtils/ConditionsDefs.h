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
 * CREATED:  March 21, 2015
 *           
 *
 *  Header file for visible classes used by TrigHLTJEtHypo
 *********************************************************************/

#include <string>
#include <memory>
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ConditionBridge.h"

using Conditions = std::vector<ConditionBridge>;
using ConditionsIter = Conditions::iterator;

#endif
