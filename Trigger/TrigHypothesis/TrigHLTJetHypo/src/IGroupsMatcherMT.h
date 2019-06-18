
/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_IGROUPSMATCHERMT_H
#define TRIGHLTJETHYPO_IGROUPSMATCHERMT_H
/********************************************************************
 *
 * NAME:     IGroupsMatcherMT.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 * AUTHOR:   P. Sherwood
 * CREATED:  March 21, 2015
 *           
 *
 *  PABC for matching gropups of jets with Conditions.
 *********************************************************************/

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJet.h"
#include "./ConditionsDefsMT.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/HypoJetDefs.h"
#include <string>

class ITrigJetHypoInfoCollector;
class xAODJetCollector;

class IGroupsMatcherMT{
 public:
// Abstract interface used by a factory function
  virtual ~IGroupsMatcherMT(){}
  virtual std::optional<bool>
    match(const HypoJetGroupCIter&,
	  const HypoJetGroupCIter&,
	  xAODJetCollector&,
	  const std::unique_ptr<ITrigJetHypoInfoCollector>&,
	  bool debug=false) const = 0;
  
  virtual ConditionsMT getConditions() const noexcept = 0;
  virtual std::string toString() const = 0;
};

#endif


