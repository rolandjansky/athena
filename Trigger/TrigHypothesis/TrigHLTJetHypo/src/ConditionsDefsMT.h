/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_CONDITIONSDEFSMT_H
#define TRIGHLTJETHYPO_CONDITIONSDEFSMT_H
/********************************************************************
 *
 * NAME:     ConditionsDefs.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 * AUTHOR:   P. Sherwood
 *********************************************************************/

#include <string>
#include <memory>
#include "./IConditionMT.h"

using ConditionsMT = std::vector<std::unique_ptr<IConditionMT>>;
using ConditionsMTIter = ConditionsMT::iterator;

#endif
