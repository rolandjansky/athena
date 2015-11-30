/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGJETHYPO_CONDITIONSDEFS_H
#define TRIGJETHYPO_CONDITIONSDEFS_H
/********************************************************************
 *
 * NAME:     ConditionsDefs.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigJetHypo
 *
 * AUTHOR:   P. Sherwood
 * CREATED:  March 21, 2015
 *           
 *
 *  Headers file for visible classes used by TrigHLTJEtHypo
 *********************************************************************/

#include "xAODJet/Jet.h"
#include <string>
#include <memory>
#include "TrigJetHypo/TrigHLTJetHypoUtils/ConditionBridge.h"

using Conditions = std::vector<ConditionBridge>;
using ConditionsIter = Conditions::iterator;

#endif
