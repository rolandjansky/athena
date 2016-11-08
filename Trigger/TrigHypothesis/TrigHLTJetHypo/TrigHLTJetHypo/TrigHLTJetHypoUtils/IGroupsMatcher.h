/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_IGROUPSMATCHER_H
#define TRIGHLTJETHYPO_IGROUPSMATCHER_H
/********************************************************************
 *
 * NAME:     IGroupsMatcher.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 * AUTHOR:   P. Sherwood
 * CREATED:  March 21, 2015
 *           
 *
 *  PABC for matching gropups of jets with Conditions.
 *********************************************************************/

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJet.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ConditionsDefs.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/HypoJetDefs.h"
#include <string>


class IGroupsMatcher{
 public:
// Abstract interface used by a factory function
  virtual ~IGroupsMatcher(){}
  virtual void match(const HypoJetGroupCIter&, const HypoJetGroupCIter&) = 0;
  virtual bool pass() const = 0;
  virtual HypoJetVector passedJets() const noexcept = 0;
  virtual HypoJetVector failedJets() const noexcept = 0;
  virtual Conditions getConditions() const noexcept = 0;
  virtual std::string toString() const noexcept = 0;
};

#endif


